#pragma once
#pragma once

#include <array>
#include <cstddef>
#include <utility>
#include "../../Foundation/GEConst.h"
#include "../../Foundation/GEModel.h"
#include "../../Foundation/GEObjectPool.h"

class GEEnemyState : public GEPoolable {
private:
    bool _active = false;
public:
    GEEnemyType type = GEEnemyType::Normal;
    float centerX = 0.0f;
    float centerY = 0.0f;
    int hp = 0;
    int maxHp = 0;
    float attackCooldown = 0.0f;

    GEEnemyState() = default;
    GEEnemyState(const GEEnemyState&) = default;
    GEEnemyState& operator=(const GEEnemyState&) = default;

    void activate() { _active = true; }
    void deactivate() { _active = false; }
    bool isActiveElement() const override { return _active; }
};

class GEPowerUpState : public GEPoolable {
private:
    bool _active = false;
public:
    GEPowerUpType type = GEPowerUpType::None;
    float centerX = 0.0f;
    float centerY = 0.0f;
    float remainingTime = 0.0f;
    float timeToLive = 0.0f;

    GEPowerUpState() = default;
    GEPowerUpState(const GEPowerUpState&) = default;
    GEPowerUpState& operator=(const GEPowerUpState&) = default;

    void activate() { _active = true; }
    void deactivate() { _active = false; }
    bool isActiveElement() const override { return _active; }
};

class GEProjectileState : public GEPoolable {
private:
    bool _active = false;
public:
    ProjectileOwner owner = ProjectileOwner::FromPlayer;
    float centerX = 0.0f;
    float centerY = 0.0f;
    float dirX = 0.0f;
    float dirY = 0.0f;
    float speed = 0.0f;
    int damage = 0;

    GEProjectileState() = default;
    GEProjectileState(const GEProjectileState&) = default;
    GEProjectileState& operator=(const GEProjectileState&) = default;

    void activate() { _active = true; }
    void deactivate() { _active = false; }
    bool isActiveElement() const override { return _active; }
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
    int aoeTargetCount = 0;
    bool aoeKeyHeld = false;
    float aoeTargetBuffTimer = 0.0f;
};

class GEEnemyManagerState {
private:
    void copyFrom(const GEEnemyManagerState& other);
    void moveFrom(GEEnemyManagerState&& other) noexcept;
public:
    float spawnTimer = 0.0f;
    float spawnInterval = 0.0f;
    float difficultyTimer = 0.0f;
    float elapsedTime = 0.0f;
    std::array<int, Enemy::ENEMY_TYPE_COUNT> killCounts{ {0, 0, 0, 0} };
    GEObjectPool<GEEnemyState*> enemyStates;

    GEEnemyManagerState();
    ~GEEnemyManagerState();
    GEEnemyManagerState(const GEEnemyManagerState& other);
    GEEnemyManagerState& operator=(const GEEnemyManagerState& other);
    GEEnemyManagerState(GEEnemyManagerState&& other) noexcept;
    GEEnemyManagerState& operator=(GEEnemyManagerState&& other) noexcept;

    void addEnemyState(const GEEnemyState& state);
    void clearEnemyStates();
    std::size_t enemyCount() const { return enemyStates.countActive(); }

};

class GEPowerUpManagerState {
private:
    void copyFrom(const GEPowerUpManagerState& other);
    void moveFrom(GEPowerUpManagerState&& other) noexcept;
public:
    float spawnTimer = 0.0f;
    GEObjectPool<GEPowerUpState*> powerUps;

    GEPowerUpManagerState();
    ~GEPowerUpManagerState();
    GEPowerUpManagerState(const GEPowerUpManagerState& other);
    GEPowerUpManagerState& operator=(const GEPowerUpManagerState& other);
    GEPowerUpManagerState(GEPowerUpManagerState&& other) noexcept;
    GEPowerUpManagerState& operator=(GEPowerUpManagerState&& other) noexcept;

    void addPowerUpState(const GEPowerUpState& state);
    void clearPowerUps();
    std::size_t powerUpCount() const { return powerUps.countActive(); }

};

class GEProjectileManagerState {
private:
    void copyFrom(const GEProjectileManagerState& other);
    void moveFrom(GEProjectileManagerState&& other) noexcept;
public:
    GEObjectPool<GEProjectileState*> projectiles;

    GEProjectileManagerState();
    ~GEProjectileManagerState();
    GEProjectileManagerState(const GEProjectileManagerState& other);
    GEProjectileManagerState& operator=(const GEProjectileManagerState& other);
    GEProjectileManagerState(GEProjectileManagerState&& other) noexcept;
    GEProjectileManagerState& operator=(GEProjectileManagerState&& other) noexcept;

    void addProjectileState(const GEProjectileState& state);
    void clearProjectiles();
    std::size_t projectileCount() const { return projectiles.countActive(); }

};

inline GEEnemyManagerState::GEEnemyManagerState() = default;

inline GEEnemyManagerState::~GEEnemyManagerState() {
    clearEnemyStates();
}

inline void GEEnemyManagerState::copyFrom(const GEEnemyManagerState& other) {
    spawnTimer = other.spawnTimer;
    spawnInterval = other.spawnInterval;
    difficultyTimer = other.difficultyTimer;
    elapsedTime = other.elapsedTime;
    killCounts = other.killCounts;

    clearEnemyStates();
    enemyStates.reserve(other.enemyStates.capacity());
    for (unsigned int i = 0; i < other.enemyStates.size(); ++i) {
        GEEnemyState* state = other.enemyStates[i];
        if (!state || !state->isActiveElement()) continue;
        GEEnemyState* copy = new GEEnemyState(*state);
        copy->activate();
        enemyStates.add(copy);
    }
}

inline void GEEnemyManagerState::moveFrom(GEEnemyManagerState&& other) noexcept {
    spawnTimer = other.spawnTimer;
    spawnInterval = other.spawnInterval;
    difficultyTimer = other.difficultyTimer;
    elapsedTime = other.elapsedTime;
    killCounts = other.killCounts;
    enemyStates = std::move(other.enemyStates);
}

inline GEEnemyManagerState::GEEnemyManagerState(const GEEnemyManagerState& other) {
    copyFrom(other);
}

inline GEEnemyManagerState& GEEnemyManagerState::operator=(const GEEnemyManagerState& other) {
    if (this != &other) {
        copyFrom(other);
    }
    return *this;
}

inline GEEnemyManagerState::GEEnemyManagerState(GEEnemyManagerState&& other) noexcept {
    moveFrom(std::move(other));
}

inline GEEnemyManagerState& GEEnemyManagerState::operator=(GEEnemyManagerState&& other) noexcept {
    if (this != &other) {
        clearEnemyStates();
        moveFrom(std::move(other));
    }
    return *this;
}

inline void GEEnemyManagerState::addEnemyState(const GEEnemyState& state) {
    GEEnemyState* copy = new GEEnemyState(state);
    copy->activate();
    enemyStates.add(copy);
}

inline void GEEnemyManagerState::clearEnemyStates() {
    enemyStates.destroyAll();
    enemyStates.clear();
}

inline GEPowerUpManagerState::GEPowerUpManagerState() = default;

inline GEPowerUpManagerState::~GEPowerUpManagerState() {
    clearPowerUps();
}

inline void GEPowerUpManagerState::copyFrom(const GEPowerUpManagerState& other) {
    spawnTimer = other.spawnTimer;
    clearPowerUps();
    powerUps.reserve(other.powerUps.capacity());
    for (unsigned int i = 0; i < other.powerUps.size(); ++i) {
        GEPowerUpState* state = other.powerUps[i];
        if (!state || !state->isActiveElement()) continue;
        GEPowerUpState* copy = new GEPowerUpState(*state);
        copy->activate();
        powerUps.add(copy);
    }
}

inline void GEPowerUpManagerState::moveFrom(GEPowerUpManagerState&& other) noexcept {
    spawnTimer = other.spawnTimer;
    powerUps = std::move(other.powerUps);
}

inline GEPowerUpManagerState::GEPowerUpManagerState(const GEPowerUpManagerState& other) {
    copyFrom(other);
}

inline GEPowerUpManagerState& GEPowerUpManagerState::operator=(const GEPowerUpManagerState& other) {
    if (this != &other) copyFrom(other);
    return *this;
}

inline GEPowerUpManagerState::GEPowerUpManagerState(GEPowerUpManagerState&& other) noexcept {
    moveFrom(std::move(other));
}

inline GEPowerUpManagerState& GEPowerUpManagerState::operator=(GEPowerUpManagerState&& other) noexcept {
    if (this != &other) {
        clearPowerUps();
        moveFrom(std::move(other));
    }
    return *this;
}

inline void GEPowerUpManagerState::addPowerUpState(const GEPowerUpState& state) {
    GEPowerUpState* copy = new GEPowerUpState(state);
    copy->activate();
    powerUps.add(copy);
}

inline void GEPowerUpManagerState::clearPowerUps() {
    powerUps.destroyAll();
    powerUps.clear();
}

inline GEProjectileManagerState::GEProjectileManagerState() = default;

inline GEProjectileManagerState::~GEProjectileManagerState() {
    clearProjectiles();
}

inline void GEProjectileManagerState::copyFrom(const GEProjectileManagerState& other) {
    clearProjectiles();
    projectiles.reserve(other.projectiles.capacity());
    for (unsigned int i = 0; i < other.projectiles.size(); ++i) {
        GEProjectileState* state = other.projectiles[i];
        if (!state || !state->isActiveElement()) continue;
        GEProjectileState* copy = new GEProjectileState(*state);
        copy->activate();
        projectiles.add(copy);
    }
}

inline void GEProjectileManagerState::moveFrom(GEProjectileManagerState&& other) noexcept {
    projectiles = std::move(other.projectiles);
}

inline GEProjectileManagerState::GEProjectileManagerState(const GEProjectileManagerState& other) {
    copyFrom(other);
}

inline GEProjectileManagerState& GEProjectileManagerState::operator=(const GEProjectileManagerState& other) {
    if (this != &other) copyFrom(other);
    return *this;
}

inline GEProjectileManagerState::GEProjectileManagerState(GEProjectileManagerState&& other) noexcept {
    moveFrom(std::move(other));
}

inline GEProjectileManagerState& GEProjectileManagerState::operator=(GEProjectileManagerState&& other) noexcept {
    if (this != &other) {
        clearProjectiles();
        moveFrom(std::move(other));
    }
    return *this;
}

inline void GEProjectileManagerState::addProjectileState(const GEProjectileState& state) {
    GEProjectileState* copy = new GEProjectileState(state);
    copy->activate();
    projectiles.add(copy);
}

inline void GEProjectileManagerState::clearProjectiles() {
    projectiles.destroyAll();
    projectiles.clear();
}
