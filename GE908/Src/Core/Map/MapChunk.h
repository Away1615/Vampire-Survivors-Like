#pragma once

struct MapChunk {
    int columns = 0;
    int rows = 0;
    int* tiles = nullptr;

    MapChunk() = default;

    ~MapChunk() {
        clear();
    }

    MapChunk(const MapChunk& other) {
        copyFrom(other);
    }

    MapChunk& operator=(const MapChunk& other) {
        if (this != &other) {
            copyFrom(other);
        }
        return *this;
    }

    MapChunk(MapChunk&& other) noexcept {
        moveFrom(other);
    }

    MapChunk& operator=(MapChunk&& other) noexcept {
        if (this != &other) {
            clear();
            moveFrom(other);
        }
        return *this;
    }

    void clear() {
        if (tiles) {
            delete[] tiles;
            tiles = nullptr;
        }
        columns = 0;
        rows = 0;
    }

    bool allocate(int cols, int rowsCount) {
        clear();
        if (cols <= 0 || rowsCount <= 0) return false;
        const int total = cols * rowsCount;
        tiles = new int[total];
        if (!tiles) return false;
        for (int i = 0; i < total; ++i) {
            tiles[i] = 0;
        }
        columns = cols;
        rows = rowsCount;
        return true;
    }

    bool isValid() const {
        return columns > 0 && rows > 0 && tiles != nullptr;
    }

    int getColumnCount() const { return columns; }
    int getRowCount() const { return rows; }

    int getTileID(int row, int col) const {
        if (!isValid()) return 0;
        if (row < 0 || row >= rows || col < 0 || col >= columns) return 0;
        const int index = row * columns + col;
        return tiles[index];
    }

    int getPixelWidth(int tileWidth) const { return columns * tileWidth; }
    int getPixelHeight(int tileHeight) const { return rows * tileHeight; }

private:
    void copyFrom(const MapChunk& other) {
        if (!other.isValid()) {
            clear();
            return;
        }
        const int total = other.columns * other.rows;
        int* newTiles = new int[total];
        for (int i = 0; i < total; ++i) {
            newTiles[i] = other.tiles[i];
        }
        clear();
        tiles = newTiles;
        columns = other.columns;
        rows = other.rows;
    }

    void moveFrom(MapChunk& other) {
        columns = other.columns;
        rows = other.rows;
        tiles = other.tiles;
        other.columns = 0;
        other.rows = 0;
        other.tiles = nullptr;
    }
};

struct ChunkCoord {
    int x = 0;
    int y = 0;
    bool operator==(const ChunkCoord& other) const { return x == other.x && y == other.y; }
};
