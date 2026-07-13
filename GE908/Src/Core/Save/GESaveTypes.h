#pragma once

#include <cstdint>
#include <string>
#include "../State/GEGameState.h"

static constexpr uint32_t GE_SAVE_FORMAT_VERSION = 1u;

struct GEMapRuntimeState {
    GEMapMode mapMode = GEMapMode::Fixed;
    bool hasRandomSeed = false;
    uint32_t randomSeed = 0u;
    int activeChunkX = 0;
    int activeChunkY = 0;
    float cameraX = 0.0f;
    float cameraY = 0.0f;
    float levelTimeRemaining = 120.0f;
};

struct GEGameSnapshot {
    GEMapRuntimeState map;
    GEPlayerState player;
    GEEnemyManagerState enemies;
    GEProjectileManagerState projectiles;
    GEPowerUpManagerState powerUps;
};

struct GESaveMetadata {
    std::string id;
    std::string displayName;
    int64_t updatedAt = 0;
    GEMapMode mapMode = GEMapMode::Fixed;
    float levelTimeRemaining = 120.0f;
    int playerHp = 0;
};

struct GESaveRecord {
    GESaveMetadata metadata;
    GEGameSnapshot snapshot;
};

enum class GESaveListMode {
    Load,
    Save,
};
