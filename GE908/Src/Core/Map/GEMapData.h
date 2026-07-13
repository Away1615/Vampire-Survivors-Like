#pragma once

#include <cstdint>
#include <string>
#include "MapChunk.h"
#include "../../Foundation/GEModel.h"

class GEMapData {
private:
    int _tileWidth = 0;
    int _tileHeight = 0;
    float _cameraOffsetX = 0.0f;
    float _cameraOffsetY = 0.0f;
    int _windowWidth = 854;
    int _windowHeight = 480;
    MapChunk _baseChunk;
    int _chunkColumns = 0;
    int _chunkRows = 0;
    bool _infiniteMap = false;
    ChunkCoord _activeChunk{ 0, 0 };
    uint32_t _randomSeed = 0u;
    bool _hasRandomSeed = false;

    bool parseKeywordLine(const std::string& line, MapChunk& chunk);
    void clear();
    static int floorDiv(int value, int divisor);
    static int positiveMod(int value, int divisor);
    bool worldToChunkIndices(int tileRow, int tileCol, ChunkCoord& coord, int& localRow, int& localCol) const;
    uint32_t hashCoordinates(int worldRow, int worldCol) const;
    int sampleTileForWorld(int worldRow, int worldCol) const;

public:
    GEMapData() = default;
    ~GEMapData();

    GEMapData(const GEMapData&) = delete;
    GEMapData& operator=(const GEMapData&) = delete;

    bool load(const std::string& filename, GEMapMode mapMode);

    int getTileWidth() const { return _tileWidth; }
    int getTileHeight() const { return _tileHeight; }

    void setCameraOffset(int x, int y);
    float getCameraOffsetX() const { return _cameraOffsetX; }
    float getCameraOffsetY() const { return _cameraOffsetY; }

    void setWindowSize(int width, int height);
    int getScreenWidth() const { return _windowWidth; }
    int getScreenHeight() const { return _windowHeight; }

    void setActiveChunk(int chunkX, int chunkY);
    void updateActiveChunkFromWorldPosition(float worldX, float worldY);
    void restoreRuntimeState(bool hasRandomSeed, uint32_t randomSeed, int activeChunkX, int activeChunkY);

    bool hasRandomSeed() const { return _hasRandomSeed; }
    uint32_t getRandomSeed() const { return _randomSeed; }
    int getActiveChunkX() const { return _activeChunk.x; }
    int getActiveChunkY() const { return _activeChunk.y; }

    const MapChunk* getActiveChunk() const;
    MapChunk* getActiveChunk();

    int getActiveChunkColumnCount() const;
    int getActiveChunkRowCount() const;
    int getActiveChunkPixelWidth() const;
    int getActiveChunkPixelHeight() const;
    int getChunkPixelWidth() const;
    int getChunkPixelHeight() const;

    bool isInfiniteMap() const { return _infiniteMap; }
    int getTileID(int row, int col) const;
};
