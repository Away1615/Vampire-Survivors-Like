#include "GEGameResources.h"

namespace {
    constexpr const char* PLAYER_FRONT_WALK_TEXTURE =
        "Src/Assets/Player/FRONT_WALK/Wizard_Front_Walk.png";
    constexpr const char* PLAYER_BACK_WALK_TEXTURE =
        "Src/Assets/Player/BACK_WALK/Wizard_Back_Walk.png";
    constexpr const char* PLAYER_SIDE_WALK_TEXTURE =
        "Src/Assets/Player/SIDE_WALK/Wizard_Side_Walk.png";
    constexpr const char* ENEMY_TEXTURES[Enemy::ENEMY_TYPE_COUNT] = {
        "Src/Assets/Textures/enemy_normal.png",
        "Src/Assets/Textures/enemy_fast.png",
        "Src/Assets/Textures/enemy_heavy.png",
        "Src/Assets/Textures/enemy_static.png",
    };
    constexpr const char* PROJECTILE_TEXTURES[2] = {
        "Src/Assets/Textures/arrow.png",
        "Src/Assets/Textures/enemy_bullet.png",
    };
    constexpr const char* POWER_UP_TEXTURES[3] = {
        "Src/Assets/Textures/attack_speed.png",
        "Src/Assets/Textures/aoe_target.png",
        "Src/Assets/Textures/heal.png",
    };
    constexpr const char* MAP_TILE_FOLDER = "Src/Assets/MapTiles/";
    constexpr const char* FONT_FOLDER = "Src/Assets/Fonts/";
}

bool GEGameResources::loadImage(Image& image, const std::string& filename) {
    image.free();
    return image.load(filename);
}

bool GEGameResources::load() {
    if (!loadImage(_playerFrontWalkTexture, PLAYER_FRONT_WALK_TEXTURE)) return false;
    if (!loadImage(_playerBackWalkTexture, PLAYER_BACK_WALK_TEXTURE)) return false;
    if (!loadImage(_playerSideWalkTexture, PLAYER_SIDE_WALK_TEXTURE)) return false;

    for (int i = 0; i < Enemy::ENEMY_TYPE_COUNT; ++i) {
        if (!loadImage(_enemyTextures[static_cast<size_t>(i)], ENEMY_TEXTURES[i])) return false;
    }

    for (int i = 0; i < static_cast<int>(_projectileTextures.size()); ++i) {
        if (!loadImage(_projectileTextures[static_cast<size_t>(i)], PROJECTILE_TEXTURES[i])) return false;
    }

    for (int i = 0; i < static_cast<int>(_powerUpTextures.size()); ++i) {
        if (!loadImage(_powerUpTextures[static_cast<size_t>(i)], POWER_UP_TEXTURES[i])) return false;
    }

    for (int i = 0; i < Map::TILES_COUNT; ++i) {
        const std::string filename = std::string(MAP_TILE_FOLDER) + std::to_string(i) + ".png";
        if (!loadImage(_mapTileTextures[static_cast<size_t>(i)], filename)) return false;
    }

    return _font.load(FONT_FOLDER);
}

const Image& GEGameResources::enemyTexture(GEEnemyType type) const {
    switch (type) {
    case GEEnemyType::Fast:
        return _enemyTextures[1];
    case GEEnemyType::Heavy:
        return _enemyTextures[2];
    case GEEnemyType::StaticShooter:
        return _enemyTextures[3];
    case GEEnemyType::Normal:
    default:
        return _enemyTextures[0];
    }
}

const Image& GEGameResources::projectileTexture(ProjectileOwner owner) const {
    return owner == ProjectileOwner::FromEnemy
        ? _projectileTextures[1]
        : _projectileTextures[0];
}

const Image& GEGameResources::powerUpTexture(GEPowerUpType type) const {
    switch (type) {
    case GEPowerUpType::AdditionalAoeTarget:
        return _powerUpTextures[1];
    case GEPowerUpType::HealPlayer:
        return _powerUpTextures[2];
    case GEPowerUpType::AttackSpeedBoost:
    default:
        return _powerUpTextures[0];
    }
}

const Image& GEGameResources::mapTileTexture(int tileID) const {
    return _mapTileTextures[static_cast<size_t>(tileID)];
}
