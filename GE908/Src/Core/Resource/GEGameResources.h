#pragma once

#include "../../Foundation/GEConst.h"
#include "../../Foundation/GEFont.h"
#include "../../Foundation/GEModel.h"
#include <array>

// Owns shared game assets.
class GEGameResources {
private:
    Image _playerFrontWalkTexture;
    Image _playerBackWalkTexture;
    Image _playerSideWalkTexture;
    std::array<Image, Enemy::ENEMY_TYPE_COUNT> _enemyTextures;
    std::array<Image, 2> _projectileTextures;
    std::array<Image, 3> _powerUpTextures;
    std::array<Image, Map::TILES_COUNT> _mapTileTextures;
    GEFont _font;

    static bool loadImage(Image& image, const std::string& filename);

public:
    GEGameResources() = default;

    bool load();

    const Image& playerFrontWalkTexture() const { return _playerFrontWalkTexture; }
    const Image& playerBackWalkTexture() const { return _playerBackWalkTexture; }
    const Image& playerSideWalkTexture() const { return _playerSideWalkTexture; }
    const Image& enemyTexture(GEEnemyType type) const;
    const Image& projectileTexture(ProjectileOwner owner) const;
    const Image& powerUpTexture(GEPowerUpType type) const;
    const Image& mapTileTexture(int tileID) const;
    const GEFont& font() const { return _font; }

    GEGameResources(const GEGameResources&) = delete;
    GEGameResources& operator=(const GEGameResources&) = delete;
    GEGameResources(GEGameResources&&) = delete;
    GEGameResources& operator=(GEGameResources&&) = delete;
};
