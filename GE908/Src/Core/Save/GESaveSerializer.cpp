#include "GESaveSerializer.h"
#include <cstring>
#include <fstream>

namespace {
    constexpr char SAVE_MAGIC[8] = { 'G', 'E', '9', '0', '8', 'S', 'A', 'V' };
    constexpr uint32_t MAX_STRING_LENGTH = 256u;

    template <typename TValue>
    bool writeValue(std::ostream& stream, const TValue& value) {
        stream.write(reinterpret_cast<const char*>(&value), sizeof(TValue));
        return stream.good();
    }

    template <typename TValue>
    bool readValue(std::istream& stream, TValue& value) {
        stream.read(reinterpret_cast<char*>(&value), sizeof(TValue));
        return stream.good();
    }

    bool writeString(std::ostream& stream, const std::string& value) {
        const uint32_t length = static_cast<uint32_t>(value.size());
        if (!writeValue(stream, length)) return false;
        stream.write(value.data(), length);
        return stream.good();
    }

    bool readString(std::istream& stream, std::string& value) {
        uint32_t length = 0u;
        if (!readValue(stream, length) || length > MAX_STRING_LENGTH) return false;
        value.resize(length);
        if (length > 0u) stream.read(value.data(), length);
        return stream.good();
    }

    bool writeBool(std::ostream& stream, bool value) {
        const uint8_t storedValue = value ? 1u : 0u;
        return writeValue(stream, storedValue);
    }

    bool readBool(std::istream& stream, bool& value) {
        uint8_t storedValue = 0u;
        if (!readValue(stream, storedValue)) return false;
        value = storedValue != 0u;
        return true;
    }

    bool writeHeader(std::ostream& stream, const GESaveMetadata& metadata) {
        stream.write(SAVE_MAGIC, sizeof(SAVE_MAGIC));
        const uint32_t version = GE_SAVE_FORMAT_VERSION;
        const int32_t mapMode = static_cast<int32_t>(metadata.mapMode);
        return stream.good()
            && writeValue(stream, version)
            && writeString(stream, metadata.id)
            && writeString(stream, metadata.displayName)
            && writeValue(stream, metadata.updatedAt)
            && writeValue(stream, mapMode)
            && writeValue(stream, metadata.levelTimeRemaining)
            && writeValue(stream, metadata.playerHp);
    }

    bool readHeader(std::istream& stream, GESaveMetadata& metadata) {
        char magic[sizeof(SAVE_MAGIC)]{};
        stream.read(magic, sizeof(magic));
        if (!stream.good() || std::memcmp(magic, SAVE_MAGIC, sizeof(SAVE_MAGIC)) != 0) return false;

        uint32_t version = 0u;
        int32_t mapMode = 0;
        if (!readValue(stream, version) || version != GE_SAVE_FORMAT_VERSION) return false;
        if (!readString(stream, metadata.id)) return false;
        if (!readString(stream, metadata.displayName)) return false;
        if (!readValue(stream, metadata.updatedAt)) return false;
        if (!readValue(stream, mapMode)) return false;
        if (!readValue(stream, metadata.levelTimeRemaining)) return false;
        if (!readValue(stream, metadata.playerHp)) return false;
        metadata.mapMode = static_cast<GEMapMode>(mapMode);
        return true;
    }

    bool writeMapState(std::ostream& stream, const GEMapRuntimeState& state) {
        const int32_t mapMode = static_cast<int32_t>(state.mapMode);
        return writeValue(stream, mapMode)
            && writeBool(stream, state.hasRandomSeed)
            && writeValue(stream, state.randomSeed)
            && writeValue(stream, state.activeChunkX)
            && writeValue(stream, state.activeChunkY)
            && writeValue(stream, state.cameraX)
            && writeValue(stream, state.cameraY)
            && writeValue(stream, state.levelTimeRemaining);
    }

    bool readMapState(std::istream& stream, GEMapRuntimeState& state) {
        int32_t mapMode = 0;
        if (!readValue(stream, mapMode)) return false;
        state.mapMode = static_cast<GEMapMode>(mapMode);
        return readBool(stream, state.hasRandomSeed)
            && readValue(stream, state.randomSeed)
            && readValue(stream, state.activeChunkX)
            && readValue(stream, state.activeChunkY)
            && readValue(stream, state.cameraX)
            && readValue(stream, state.cameraY)
            && readValue(stream, state.levelTimeRemaining);
    }

    bool writePlayerState(std::ostream& stream, const GEPlayerState& state) {
        return writeValue(stream, state.centerX)
            && writeValue(stream, state.centerY)
            && writeValue(stream, state.hp)
            && writeValue(stream, state.maxHp)
            && writeValue(stream, state.speed)
            && writeValue(stream, state.autoAttackTimer)
            && writeValue(stream, state.autoAttackSpeedMultiplier)
            && writeValue(stream, state.attackSpeedBuffTimer)
            && writeValue(stream, state.aoeCooldownTimer)
            && writeValue(stream, state.aoeCooldown)
            && writeValue(stream, state.contactDamageCooldownTimer)
            && writeValue(stream, state.aoeTargetCount)
            && writeBool(stream, state.aoeKeyHeld)
            && writeValue(stream, state.aoeTargetBuffTimer);
    }

    bool readPlayerState(std::istream& stream, GEPlayerState& state) {
        return readValue(stream, state.centerX)
            && readValue(stream, state.centerY)
            && readValue(stream, state.hp)
            && readValue(stream, state.maxHp)
            && readValue(stream, state.speed)
            && readValue(stream, state.autoAttackTimer)
            && readValue(stream, state.autoAttackSpeedMultiplier)
            && readValue(stream, state.attackSpeedBuffTimer)
            && readValue(stream, state.aoeCooldownTimer)
            && readValue(stream, state.aoeCooldown)
            && readValue(stream, state.contactDamageCooldownTimer)
            && readValue(stream, state.aoeTargetCount)
            && readBool(stream, state.aoeKeyHeld)
            && readValue(stream, state.aoeTargetBuffTimer);
    }

    bool writeEnemyState(std::ostream& stream, const GEEnemyManagerState& state) {
        if (!writeValue(stream, state.spawnTimer)
            || !writeValue(stream, state.spawnInterval)
            || !writeValue(stream, state.difficultyTimer)
            || !writeValue(stream, state.elapsedTime)) return false;

        for (int value : state.killCounts) {
            if (!writeValue(stream, value)) return false;
        }

        const uint32_t count = static_cast<uint32_t>(state.enemyCount());
        if (!writeValue(stream, count)) return false;
        for (uint32_t index = 0; index < count; ++index) {
            const GEEnemyState* enemy = state.enemyStates[index];
            const int32_t type = static_cast<int32_t>(enemy->type);
            if (!writeValue(stream, type)
                || !writeValue(stream, enemy->centerX)
                || !writeValue(stream, enemy->centerY)
                || !writeValue(stream, enemy->hp)
                || !writeValue(stream, enemy->maxHp)
                || !writeValue(stream, enemy->attackCooldown)) return false;
        }
        return true;
    }

    bool readEnemyState(std::istream& stream, GEEnemyManagerState& state) {
        state.clearEnemyStates();
        if (!readValue(stream, state.spawnTimer)
            || !readValue(stream, state.spawnInterval)
            || !readValue(stream, state.difficultyTimer)
            || !readValue(stream, state.elapsedTime)) return false;

        for (int& value : state.killCounts) {
            if (!readValue(stream, value)) return false;
        }

        uint32_t count = 0u;
        if (!readValue(stream, count) || count > Enemy::MAX_ENEMIES) return false;
        for (uint32_t index = 0; index < count; ++index) {
            GEEnemyState enemy;
            int32_t type = 0;
            if (!readValue(stream, type)
                || !readValue(stream, enemy.centerX)
                || !readValue(stream, enemy.centerY)
                || !readValue(stream, enemy.hp)
                || !readValue(stream, enemy.maxHp)
                || !readValue(stream, enemy.attackCooldown)) return false;
            enemy.type = static_cast<GEEnemyType>(type);
            enemy.activate();
            state.addEnemyState(enemy);
        }
        return true;
    }

    bool writeProjectileState(std::ostream& stream, const GEProjectileManagerState& state) {
        const uint32_t count = static_cast<uint32_t>(state.projectileCount());
        if (!writeValue(stream, count)) return false;
        for (uint32_t index = 0; index < count; ++index) {
            const GEProjectileState* projectile = state.projectiles[index];
            const int32_t owner = static_cast<int32_t>(projectile->owner);
            if (!writeValue(stream, owner)
                || !writeValue(stream, projectile->centerX)
                || !writeValue(stream, projectile->centerY)
                || !writeValue(stream, projectile->dirX)
                || !writeValue(stream, projectile->dirY)
                || !writeValue(stream, projectile->speed)
                || !writeValue(stream, projectile->damage)) return false;
        }
        return true;
    }

    bool readProjectileState(std::istream& stream, GEProjectileManagerState& state) {
        state.clearProjectiles();
        uint32_t count = 0u;
        if (!readValue(stream, count) || count > Projectile::MAX_PROJECTILES) return false;
        for (uint32_t index = 0; index < count; ++index) {
            GEProjectileState projectile;
            int32_t owner = 0;
            if (!readValue(stream, owner)
                || !readValue(stream, projectile.centerX)
                || !readValue(stream, projectile.centerY)
                || !readValue(stream, projectile.dirX)
                || !readValue(stream, projectile.dirY)
                || !readValue(stream, projectile.speed)
                || !readValue(stream, projectile.damage)) return false;
            projectile.owner = static_cast<ProjectileOwner>(owner);
            projectile.activate();
            state.addProjectileState(projectile);
        }
        return true;
    }

    bool writePowerUpState(std::ostream& stream, const GEPowerUpManagerState& state) {
        if (!writeValue(stream, state.spawnTimer)) return false;
        const uint32_t count = static_cast<uint32_t>(state.powerUpCount());
        if (!writeValue(stream, count)) return false;
        for (uint32_t index = 0; index < count; ++index) {
            const GEPowerUpState* powerUp = state.powerUps[index];
            const int32_t type = static_cast<int32_t>(powerUp->type);
            if (!writeValue(stream, type)
                || !writeValue(stream, powerUp->centerX)
                || !writeValue(stream, powerUp->centerY)
                || !writeValue(stream, powerUp->remainingTime)
                || !writeValue(stream, powerUp->timeToLive)) return false;
        }
        return true;
    }

    bool readPowerUpState(std::istream& stream, GEPowerUpManagerState& state) {
        state.clearPowerUps();
        if (!readValue(stream, state.spawnTimer)) return false;
        uint32_t count = 0u;
        if (!readValue(stream, count) || count > 100u) return false;
        for (uint32_t index = 0; index < count; ++index) {
            GEPowerUpState powerUp;
            int32_t type = 0;
            if (!readValue(stream, type)
                || !readValue(stream, powerUp.centerX)
                || !readValue(stream, powerUp.centerY)
                || !readValue(stream, powerUp.remainingTime)
                || !readValue(stream, powerUp.timeToLive)) return false;
            powerUp.type = static_cast<GEPowerUpType>(type);
            powerUp.activate();
            state.addPowerUpState(powerUp);
        }
        return true;
    }
}

bool GESaveSerializer::write(const std::filesystem::path& filename, const GESaveRecord& record) {
    std::ofstream file(filename, std::ios::binary | std::ios::trunc);
    if (!file.is_open()) return false;

    const bool written = writeHeader(file, record.metadata)
        && writeMapState(file, record.snapshot.map)
        && writePlayerState(file, record.snapshot.player)
        && writeEnemyState(file, record.snapshot.enemies)
        && writeProjectileState(file, record.snapshot.projectiles)
        && writePowerUpState(file, record.snapshot.powerUps);
    file.flush();
    return written && file.good();
}

bool GESaveSerializer::readMetadata(const std::filesystem::path& filename, GESaveMetadata& metadata) {
    std::ifstream file(filename, std::ios::binary);
    return file.is_open() && readHeader(file, metadata);
}

bool GESaveSerializer::read(const std::filesystem::path& filename, GESaveRecord& record) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open() || !readHeader(file, record.metadata)) return false;
    return readMapState(file, record.snapshot.map)
        && readPlayerState(file, record.snapshot.player)
        && readEnemyState(file, record.snapshot.enemies)
        && readProjectileState(file, record.snapshot.projectiles)
        && readPowerUpState(file, record.snapshot.powerUps);
}
