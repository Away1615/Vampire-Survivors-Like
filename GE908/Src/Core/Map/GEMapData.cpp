#include "GEMapData.h"
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <sstream>

GEMapData::~GEMapData() {
    clear();
}

bool GEMapData::parseKeywordLine(const std::string& line, MapChunk& chunk) {
    if (line.empty()) return false;

    std::istringstream stream(line);
    std::string key;
    if (!(stream >> key)) return false;

    if (key == "seed") {
        uint32_t flagOrSeed = 0u;
        uint32_t value = 0u;
        if (!(stream >> flagOrSeed)) return false;
        if (stream >> value) {
            _hasRandomSeed = flagOrSeed != 0u;
            _randomSeed = value;
        }
        else {
            _randomSeed = flagOrSeed;
            _hasRandomSeed = true;
        }
        return true;
    }

    int value = 0;
    if (!(stream >> value)) return false;

    if (key == "tileswide") chunk.columns = value;
    else if (key == "tileshigh") chunk.rows = value;
    else if (key == "tilewidth") _tileWidth = value;
    else if (key == "tileheight") _tileHeight = value;
    return true;
}

void GEMapData::clear() {
    _baseChunk.clear();
    _tileWidth = 0;
    _tileHeight = 0;
    _cameraOffsetX = 0.0f;
    _cameraOffsetY = 0.0f;
    _chunkColumns = 0;
    _chunkRows = 0;
    _infiniteMap = false;
    _activeChunk = { 0, 0 };
    _randomSeed = 0u;
    _hasRandomSeed = false;
}

int GEMapData::floorDiv(int value, int divisor) {
    int quotient = value / divisor;
    const int remainder = value % divisor;
    if (remainder != 0 && ((remainder < 0) != (divisor < 0))) --quotient;
    return quotient;
}

int GEMapData::positiveMod(int value, int divisor) {
    int result = value % divisor;
    if (result < 0) result += std::abs(divisor);
    return result;
}

bool GEMapData::worldToChunkIndices(
    int tileRow,
    int tileCol,
    ChunkCoord& coord,
    int& localRow,
    int& localCol) const {
    if (_chunkColumns <= 0 || _chunkRows <= 0) return false;
    coord.x = floorDiv(tileCol, _chunkColumns);
    coord.y = floorDiv(tileRow, _chunkRows);
    localCol = positiveMod(tileCol, _chunkColumns);
    localRow = positiveMod(tileRow, _chunkRows);
    return true;
}

uint32_t GEMapData::hashCoordinates(int worldRow, int worldCol) const {
    const uint32_t prime = 101u;
    uint32_t hash = _randomSeed;
    hash = hash * prime + worldRow;
    hash = hash * prime + worldCol;
    return hash;
}

int GEMapData::sampleTileForWorld(int worldRow, int worldCol) const {
    if (!_baseChunk.isValid()) return 0;
    if (!_hasRandomSeed) {
        const int sourceRow = positiveMod(worldRow, _chunkRows);
        const int sourceColumn = positiveMod(worldCol, _chunkColumns);
        return _baseChunk.getTileID(sourceRow, sourceColumn);
    }

    const uint32_t hash = hashCoordinates(worldRow, worldCol);
    const int sourceRow = static_cast<int>(hash % static_cast<uint32_t>(_chunkRows));
    const int sourceColumn = static_cast<int>(
        (hash / static_cast<uint32_t>(_chunkRows)) % static_cast<uint32_t>(_chunkColumns));
    return _baseChunk.getTileID(sourceRow, sourceColumn);
}

void GEMapData::setCameraOffset(float x, float y) {
    _cameraOffsetX = x;
    _cameraOffsetY = y;
}

void GEMapData::setWindowSize(int width, int height) {
    _windowWidth = width;
    _windowHeight = height;
}

void GEMapData::setActiveChunk(int chunkX, int chunkY) {
    if (!_infiniteMap) {
        _activeChunk = { 0, 0 };
        return;
    }
    _activeChunk = { chunkX, chunkY };
}

void GEMapData::updateActiveChunkFromWorldPosition(float worldX, float worldY) {
    if (!_infiniteMap || _tileWidth <= 0 || _tileHeight <= 0) return;

    const int tileColumn = static_cast<int>(std::floor(worldX / _tileWidth));
    const int tileRow = static_cast<int>(std::floor(worldY / _tileHeight));
    ChunkCoord coordinate;
    int localRow = 0;
    int localColumn = 0;
    if (!worldToChunkIndices(tileRow, tileColumn, coordinate, localRow, localColumn)) return;
    setActiveChunk(coordinate.x, coordinate.y);
}

void GEMapData::restoreRuntimeState(
    bool hasRandomSeed,
    uint32_t randomSeed,
    int activeChunkX,
    int activeChunkY) {
    _hasRandomSeed = hasRandomSeed;
    _randomSeed = randomSeed;
    setActiveChunk(activeChunkX, activeChunkY);
}

const MapChunk* GEMapData::getActiveChunk() const {
    return _baseChunk.isValid() ? &_baseChunk : nullptr;
}

MapChunk* GEMapData::getActiveChunk() {
    return _baseChunk.isValid() ? &_baseChunk : nullptr;
}

int GEMapData::getActiveChunkColumnCount() const {
    const MapChunk* chunk = getActiveChunk();
    return chunk ? chunk->getColumnCount() : 0;
}

int GEMapData::getActiveChunkRowCount() const {
    const MapChunk* chunk = getActiveChunk();
    return chunk ? chunk->getRowCount() : 0;
}

int GEMapData::getActiveChunkPixelWidth() const {
    const MapChunk* chunk = getActiveChunk();
    return chunk ? chunk->getPixelWidth(_tileWidth) : 0;
}

int GEMapData::getActiveChunkPixelHeight() const {
    const MapChunk* chunk = getActiveChunk();
    return chunk ? chunk->getPixelHeight(_tileHeight) : 0;
}

int GEMapData::getChunkPixelWidth() const {
    return _chunkColumns * _tileWidth;
}

int GEMapData::getChunkPixelHeight() const {
    return _chunkRows * _tileHeight;
}

bool GEMapData::load(const std::string& filename, GEMapMode mapMode) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    clear();
    _infiniteMap = mapMode == GEMapMode::Infinite;

    std::string line;
    int currentRow = 0;
    bool readingLayer = false;
    MapChunk chunk;

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        if (!readingLayer && line.find("layer") != 0) {
            parseKeywordLine(line, chunk);
            continue;
        }
        if (line.find("layer 0") == 0) {
            currentRow = 0;
            readingLayer = true;
            if (chunk.columns <= 0 || chunk.rows <= 0) return false;
            if (!chunk.allocate(chunk.columns, chunk.rows)) return false;
            continue;
        }

        if (!readingLayer) continue;
        if (currentRow >= chunk.rows) {
            readingLayer = false;
            continue;
        }

        std::stringstream rowStream(line);
        std::string cell;
        int column = 0;
        while (std::getline(rowStream, cell, ',') && column < chunk.columns) {
            int value = 0;
            try {
                value = std::stoi(cell);
            }
            catch (...) {
                value = 0;
            }
            chunk.tiles[currentRow * chunk.columns + column] = value;
            ++column;
        }
        ++currentRow;
        if (currentRow >= chunk.rows) readingLayer = false;
    }

    if (!chunk.isValid()) return false;

    _baseChunk = chunk;
    _chunkColumns = chunk.getColumnCount();
    _chunkRows = chunk.getRowCount();
    if (_chunkColumns <= 0 || _chunkRows <= 0) _infiniteMap = false;

    if (_infiniteMap && !_hasRandomSeed) {
        _randomSeed = static_cast<uint32_t>(std::rand());
        _hasRandomSeed = true;
    }

    _activeChunk = { 0, 0 };
    return true;
}

int GEMapData::getTileID(int row, int col) const {
    if (_infiniteMap && _chunkColumns > 0 && _chunkRows > 0) {
        return sampleTileForWorld(row, col);
    }
    const MapChunk* chunk = getActiveChunk();
    return chunk ? chunk->getTileID(row, col) : 0;
}
