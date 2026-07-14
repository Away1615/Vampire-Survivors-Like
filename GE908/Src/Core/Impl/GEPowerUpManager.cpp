#include "GEPowerUpManager.h"
#include "../Resource/GEGameResources.h"
#include "../../Foundation/GEUtility.h"
#include <ctime>

GEPowerUpManager::GEPowerUpManager(const GEGameResources& resources)
    : _resources(resources),
    _randomState(static_cast<uint32_t>(std::time(nullptr)) ^ 0xA511E9B3u) {
    _powerUps.reset(PowerUp::MAX_POWERUPS);
}

GEPowerUpManager::~GEPowerUpManager() = default;

void GEPowerUpManager::load(GEMapData* mapData) {
    reset();
    _mapData = mapData;
}

void GEPowerUpManager::reset() {
    _mapData = nullptr;
    _powerUps.reset(PowerUp::MAX_POWERUPS);
}

void GEPowerUpManager::spawnPowerUpAt(const GEPoint& point) {
    if (randomFloat(_randomState, 0.0f, 1.0f) > PowerUp::DROP_CHANCE) return;

    float offsetX = randomFloat(_randomState, -20.0f, 20.0f);
    float offsetY = randomFloat(_randomState, -20.0f, 20.0f);
    float spawnX = point.x + offsetX;
    float spawnY = point.y + offsetY;

    float randomN = randomFloat(_randomState, 0.0f, 1.0f);

    GEPowerUpType type = GEPowerUpType::AttackSpeedBoost;
    if (randomN < 0.33) type = GEPowerUpType::AttackSpeedBoost;
    if (randomN >= 0.33 && randomN <= 0.66) type = GEPowerUpType::AdditionalAoeTarget;
    if (randomN > 0.66) type = GEPowerUpType::HealPlayer;

    // Reuse inactive power-ups.
    const Image& texture = _resources.powerUpTexture(type);
    for (unsigned int i = 0; i < _powerUps.size(); ++i) {
        GEPowerUp* powerUp = _powerUps.getAt(i);
        if (!powerUp) {
            powerUp = new GEPowerUp(texture);
            _powerUps.add(powerUp);
        }
        if (!powerUp->isAlive()) {
            powerUp->spawn(type, texture, spawnX, spawnY, PowerUp::POWERUP_LIFETIME_SECONDS);
            return;
        }
    }
}

void GEPowerUpManager::update(float deltaTime, PlayerProvider& player) {
    if (!_mapData) return;

    for (unsigned int i = 0; i < _powerUps.size(); ++i) {
        GEPowerUp* p = _powerUps.getAt(i);
        if (!p || !p->isAlive()) continue;
        p->update(deltaTime);

        if (p->isAlive() && p->collide(player.collisionBody())) {
            player.applyPowerUp(p->getType());
            p->deactivate();
        }
    }

}

void GEPowerUpManager::draw(Window& window, const GECamera& camera) {
    for (unsigned int i = 0; i < _powerUps.size(); ++i) {
        GEPowerUp* p = _powerUps.getAt(i);
        if (p && p->isAlive())
            p->draw(window, camera);
    }
}

void GEPowerUpManager::onEnemyDefeated(const GEPoint& position) {
    spawnPowerUpAt(position);
}

GEPowerUpManagerState GEPowerUpManager::snapshotState() const {
    GEPowerUpManagerState state;
    state.randomState = _randomState;
    for (unsigned int i = 0; i < _powerUps.size(); ++i) {
        GEPowerUp* powerUp = _powerUps.getAt(i);
        if (!powerUp || !powerUp->isAlive()) continue;
        GEPowerUpState powerUpState = powerUp->snapshotState();
        state.addPowerUpState(powerUpState);
    }
    return state;
}

void GEPowerUpManager::applyState(const GEPowerUpManagerState& state) {
    _randomState = state.randomState;
    _powerUps.reset(PowerUp::MAX_POWERUPS);

    for (unsigned int i = 0; i < state.powerUps.size(); ++i) {
        const GEPowerUpState& powerUpState = state.powerUps[i];
        GEPowerUp* powerUp = new GEPowerUp(
            _resources.powerUpTexture(powerUpState.type));
        powerUp->applyState(powerUpState);
        _powerUps.add(powerUp);
    }
}
