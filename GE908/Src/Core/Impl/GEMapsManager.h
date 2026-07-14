#pragma once
#include "../../../ThirdParty/GamesEngineeringBase.h"
#include "../../Foundation/GECollisible.h"
#include "../../Foundation/GEConst.h"
#include "../Map/GEMapData.h"
#include "../Interface/GEProvider.h"
#include "../Entity/GETile.h"
#include <array>
#include <memory>

using namespace GamesEngineeringBase;

class GEGameResources;

// Loads and renders map tiles.
class GEMapsManager : public MapProvider {
private:
    std::array<std::unique_ptr<GETile>, Map::TILES_COUNT> _tiles;
    GEMapData* _mapData = nullptr;
    GETile* getTile(int tileID) const;
public:

    explicit GEMapsManager(const GEGameResources& resources);
    ~GEMapsManager() = default;

    bool load(GEMapData* mapData, GEMapMode mapMode) override;

    void reset() override { _mapData = nullptr; }

    GECollisionLayer getTileCollisionLayer(int tileID) const override;

    GEMapData* getMapData() const { return _mapData; }

    void draw(Window& window, const GECamera& camera) const;
};

