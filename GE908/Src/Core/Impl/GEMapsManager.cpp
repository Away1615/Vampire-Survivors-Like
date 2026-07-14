#include "GEMapsManager.h"
#include "../Resource/GEGameResources.h"
#include <cmath>

static constexpr const char* MAP_FILE_PATH = "Src/Assets/Maps/tilt.txt";

static GECollisionLayer TileCollisionLayer(int tileID) {
	if (tileID <= 13 || tileID == 23) return GECollisionLayer::None;
	if (tileID == 24) return GECollisionLayer::TerrainHazard;
	return GECollisionLayer::TerrainSolid;
}

GEMapsManager::GEMapsManager(const GEGameResources& resources) {
	for (int i = 0; i < Map::TILES_COUNT; ++i) {
		_tiles[static_cast<size_t>(i)] = std::make_unique<GETile>(
			resources.mapTileTexture(i),
			TileCollisionLayer(i));
	}
}

bool GEMapsManager::load(GEMapData* mapData, GEMapMode mapMode) {
	if (!mapData || !mapData->load(MAP_FILE_PATH, mapMode)) return false;
	_mapData = mapData;

	for (int i = 0; i < Map::TILES_COUNT; ++i) {
		_tiles[static_cast<size_t>(i)]->setCollisionSize(
			static_cast<float>(_mapData->getTileWidth()),
			static_cast<float>(_mapData->getTileHeight()));
	}
	return true;
}

GETile* GEMapsManager::getTile(int tileID) const {
	if (tileID < 0 || tileID >= Map::TILES_COUNT) return nullptr;
	return _tiles[static_cast<size_t>(tileID)].get();
}

GECollisionLayer GEMapsManager::getTileCollisionLayer(int tileID) const {
	GETile* tile = getTile(tileID);
	return tile ? tile->getCollisionLayer() : GECollisionLayer::None;
}

void GEMapsManager::draw(Window& window, const GECamera& camera) const {
    if (!_mapData) return;

	const int tileWidth = _mapData->getTileWidth();
	const int tileHeight = _mapData->getTileHeight();

	if (tileWidth <= 0 || tileHeight <= 0) return;

	const float cameraX = camera.getX();
	const float cameraY = camera.getY();
	const float cameraRight = cameraX + static_cast<float>(camera.getWidth());
	const float cameraBottom = cameraY + static_cast<float>(camera.getHeight());

	int minCol = static_cast<int>(std::floor(cameraX / tileWidth));
	int maxCol = static_cast<int>(std::floor((cameraRight - 1.0f) / tileWidth));
	int minRow = static_cast<int>(std::floor(cameraY / tileHeight));
	int maxRow = static_cast<int>(std::floor((cameraBottom - 1.0f) / tileHeight));

	// Pad the view to avoid edge gaps.
	minCol -= 1;
	minRow -= 1;
	maxCol += 1;
	maxRow += 1;

	for (int rowNumber = minRow; rowNumber <= maxRow; rowNumber++) {
		for (int colNumber = minCol; colNumber <= maxCol; colNumber++) {
			int tileID = _mapData->getTileID(rowNumber, colNumber);
			GETile* tile = getTile(tileID);
			if (!tile) continue;
			const float tileCenterX = (colNumber + 0.5f) * tileWidth;
			const float tileCenterY = (rowNumber + 0.5f) * tileHeight;
			tile->setCenter(tileCenterX, tileCenterY);
			tile->draw(window, camera);
		}
	}
}
