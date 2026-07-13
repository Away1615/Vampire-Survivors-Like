#pragma once
#include <string>
#include "../../../ThirdParty/GamesEngineeringBase.h"
#include "../../Foundation/GECollisible.h"
#include "../../Foundation/GEConst.h"
#include "../Map/GEMapData.h"
#include "../Interface/GEProvider.h"
#include "../Entity/GETile.h"

using namespace GamesEngineeringBase;

class GEMapsManager : public MapProvider {
private:
    GETile** _tiles = nullptr;// image cache
    int _tileCount = 0;
    GEMapData* _mapData = nullptr;
public:

    GEMapsManager();
    ~GEMapsManager();

    // load all tiles images from folder
    void loadTileResources(const std::string& folderPath);

    void load(GEMapData* mapData);

    void reset() override { _mapData = nullptr; }

    // get tile Image
    GETile* getTile(int tileID) const;
    GECollisionLayer getTileCollisionLayer(int tileID) const override;

    GEMapData* getMapData() const { return _mapData; }

    void draw(Window& window, const GECamera& camera) const;
};

