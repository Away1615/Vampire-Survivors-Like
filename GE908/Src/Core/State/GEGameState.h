#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "../../Foundation/GEConst.h"
#include "../../Foundation/GEModel.h"

struct GEEnemyState {
    GEEnemyType type = GEEnemyType::Normal;
    float centerX = 0.0f;
    float centerY = 0.0f;
    int hp = 0;
    int maxHp = 0;
    float attackCooldown = 0.0f;
    float contactDamageCooldownTimer = 0.0f;
};

struct GEPowerUpState {
    GEPowerUpType type = GEPowerUpType::None;
    float centerX = 0.0f;
    float centerY = 0.0f;
    float remainingTime = 0.0f;
    float timeToLive = 0.0f;
};

struct GEProjectileState {
    ProjectileOwner owner = ProjectileOwner::FromPlayer;
    float centerX = 0.0f;
    float centerY = 0.0f;
    float dirX = 0.0f;
    float dirY = 0.0f;
    float speed = 0.0f;
    int damage = 0;
};

struct GEPlayerState {
    float centerX = 0.0f;
    float centerY = 0.0f;
    int hp = 0;
    int maxHp = 0;
    int speed = 0;
    float autoAttackTimer = 0.0f;
    float autoAttackSpeedMultiplier = 1.0f;
    float attackSpeedBuffTimer = 0.0f;
    float aoeCooldownTimer = 0.0f;
    float aoeCooldown = 0.0f;
    float contactDamageCooldownTimer = 0.0f;
    bool wasInFire = false;
    float fireTimer = 0.0f;
    int aoeTargetCount = 0;
    bool aoeKeyHeld = false;
    float aoeTargetBuffTimer = 0.0f;
};

struct GEEnemyManagerState {
    float spawnTimer = 0.0f;
    float spawnInterval = 0.0f;
    float difficultyTimer = 0.0f;
    float elapsedTime = 0.0f;
    uint32_t randomState = 1u;
    std::array<int, Enemy::ENEMY_TYPE_COUNT> killCounts{ {0, 0, 0, 0} };
    std::vector<GEEnemyState> enemyStates;

    void addEnemyState(const GEEnemyState& state) { enemyStates.push_back(state); }
    void clearEnemyStates() { enemyStates.clear(); }
    std::size_t enemyCount() const { return enemyStates.size(); }
};

struct GEPowerUpManagerState {
    uint32_t randomState = 1u;
    std::vector<GEPowerUpState> powerUps;

    void addPowerUpState(const GEPowerUpState& state) { powerUps.push_back(state); }
    void clearPowerUps() { powerUps.clear(); }
    std::size_t powerUpCount() const { return powerUps.size(); }
};

struct GEProjectileManagerState {
    std::vector<GEProjectileState> projectiles;

    void addProjectileState(const GEProjectileState& state) { projectiles.push_back(state); }
    void clearProjectiles() { projectiles.clear(); }
    std::size_t projectileCount() const { return projectiles.size(); }
};
