#include "GEEnemyManager.h"
#include "../Resource/GEGameResources.h"
#include <ctime>

GEEnemyManager::GEEnemyManager(const GEGameResources& resources)
    : _resources(resources),
    _randomState(static_cast<uint32_t>(std::time(nullptr)) ^ 0x9E3779B9u) {
    _enemies.reset(Enemy::MAX_ENEMIES);
    resetKillCounts();
}

GEEnemyManager::~GEEnemyManager() = default;

void GEEnemyManager::reset() {
    _mapData = nullptr;
    _spawnInterval = Enemy::DEFAULT_SPAWN_INTERVAL;
    _spawnTimer = 0.0f;
    _difficultyTimer = 0.0f;
    _elapsedTime = 0.0f;
    _activeEnemies.clear();

    _enemies.reset(Enemy::MAX_ENEMIES);
    resetKillCounts();
}

void GEEnemyManager::load(GEMapData* mapData) {
    reset();
    _mapData = mapData;
}

bool GEEnemyManager::spawnEnemyOutsideCamera(PlayerProvider& player) {
    if (getEnemyCount() >= Enemy::MAX_ENEMIES) return false;

    float camOffsetX = _mapData->getCameraOffsetX();
    float camOffsetY = _mapData->getCameraOffsetY();
    int screenW = _mapData->getScreenWidth();
    int screenH = _mapData->getScreenHeight();
    int mapW = _mapData->getActiveChunkPixelWidth();
    int mapH = _mapData->getActiveChunkPixelHeight();
    const bool infinite = _mapData->isInfiniteMap();

    if (!infinite && (mapW <= 0 || mapH <= 0)) return false;

    const int safeDistance = _mapData->getTileWidth() * 2;
    int side = randomIndex(_randomState, 4);
    float x = 0, y = 0;

    switch (side) {
    case 0:
        x = camOffsetX + randomIndex(_randomState, screenW);
        y = camOffsetY - safeDistance; 
        break;            
    case 1: 
        x = camOffsetX + randomIndex(_randomState, screenW);
        y = camOffsetY + screenH + safeDistance; 
        break;
    case 2: 
        x = camOffsetX - safeDistance;
        y = camOffsetY + randomIndex(_randomState, screenH);
        break;            
    case 3:
        x = camOffsetX + screenW + safeDistance;
        y = camOffsetY + randomIndex(_randomState, screenH);
        break;  
    }

    if (!infinite) {
        x = clamp(x, 0.0f, static_cast<float>(mapW - 1));
        y = clamp(y, 0.0f, static_cast<float>(mapH - 1));
    }

    const GEEnemyType type = static_cast<GEEnemyType>(
        randomIndex(_randomState, Enemy::ENEMY_TYPE_COUNT));
    const Image& texture = _resources.enemyTexture(type);
    GEEnemy* enemy = _enemies.findInactive();
    if (enemy) {
        enemy->spawn(type, texture, x, y, mapW, mapH, infinite);
    }
    else {
        enemy = new GEEnemy(type, texture);
        enemy->spawn(type, texture, x, y, mapW, mapH, infinite);
        _enemies.add(enemy);
    }
    _activeEnemies.push_back(enemy);
    return true;
}

void GEEnemyManager::rebuildActiveEnemies() {
    _activeEnemies.clear();
    _activeEnemies.reserve(_enemies.countActive());
    for (unsigned int i = 0; i < _enemies.size(); ++i) {
        GEEnemy* enemy = _enemies.getAt(i);
        if (enemy && enemy->isAlive()) _activeEnemies.push_back(enemy);
    }
}

void GEEnemyManager::draw(Window& window, const GECamera& camera) {
    for (unsigned int i = 0; i < _activeEnemies.size(); ++i) {
        GEEnemy* enemy = _activeEnemies[i];
        if (enemy && enemy->isAlive())
            enemy->draw(window, camera);
    }
}

void GEEnemyManager::update(float deltaTime,
    PlayerProvider& player,
    ProjectileProvider& projectileProvider,
    PowerUpProvider& powerUpProvider) {
    _spawnTimer += deltaTime;
    _difficultyTimer += deltaTime;
    _elapsedTime += deltaTime;

    const int capSteps = static_cast<int>(_elapsedTime / Enemy::ACTIVE_ENEMY_CAP_STEP_TIME);
    const int activeEnemyCap = min(
        Enemy::MAX_ENEMIES,
        Enemy::BASE_ACTIVE_ENEMY_CAP + capSteps * Enemy::ACTIVE_ENEMY_CAP_INCREMENT
    );

    if (getEnemyCount() < activeEnemyCap) {
        while (_spawnTimer >= _spawnInterval) {
            if (!spawnEnemyOutsideCamera(player)) break;
            _spawnTimer -= _spawnInterval;
        }
    }

    if (_difficultyTimer >= Enemy::SPAWN_DIFFICULTY_STEP_TIME) {
        _difficultyTimer -= Enemy::SPAWN_DIFFICULTY_STEP_TIME;
        _spawnInterval = max(_spawnInterval - Enemy::SPAWN_INTERVAL_STEP, Enemy::MIN_SPAWN_INTERVAL);
    }

    GECollisible& playerBody = player.collisionBody();

    for (unsigned int i = 0; i < _activeEnemies.size();) {
        GEEnemy* enemy = _activeEnemies[i];
        if (!enemy) {
            ++i;
            continue;
        }
        if (!enemy->isAlive()) {
            if (!settleEnemyDefeat(*enemy, powerUpProvider)) ++i;
            continue;
        }

        enemy->update(
            deltaTime,
            playerBody.getCenterX(),
            playerBody.getCenterY(),
            projectileProvider);

        if (enemy->collide(playerBody)) {
            if (player.canReceiveContactDamage()) {
                player.takeDamage(Enemy::PLAYER_COLLISION_DAMAGE);
                player.startContactDamageCooldown();
            }
            if (enemy->canReceiveContactDamage()) {
                enemy->takeDamage(Enemy::ENEMY_COLLISION_DAMAGE);
                enemy->startContactDamageCooldown();
            }
        }

        if (!enemy->isAlive()) {
            if (!settleEnemyDefeat(*enemy, powerUpProvider)) ++i;
            continue;
        }
        ++i;
    }

    if (_mapData) {
        const float playerX = playerBody.getCenterX();
        const float playerY = playerBody.getCenterY();
        _mapData->updateActiveChunkFromWorldPosition(playerX, playerY);
    }
}

bool GEEnemyManager::settleEnemyDefeat(GEEnemy& enemy, PowerUpProvider& powerUpProvider) {
    if (enemy.isAlive()) return false;

    // The active list guards one-time settlement.
    for (unsigned int i = 0; i < _activeEnemies.size(); ++i) {
        if (_activeEnemies[i] != &enemy) continue;

        recordEnemyKill(enemy.getType());
        powerUpProvider.onEnemyDefeated(GEPoint(enemy.getCenterX(), enemy.getCenterY()));
        _activeEnemies[i] = _activeEnemies.back();
        _activeEnemies.pop_back();
        return true;
    }
    return false;
}

void GEEnemyManager::recordEnemyKill(GEEnemyType type) {
    int index = static_cast<int>(type);
    if (index >= 0 && index < Enemy::ENEMY_TYPE_COUNT)
        ++_killCounts[index];
}

void GEEnemyManager::resetKillCounts() {
    for (int& k : _killCounts) k = 0;
}

int GEEnemyManager::getKillCount(GEEnemyType type) const {
    int index = static_cast<int>(type);
    return (index >= 0 && index < Enemy::ENEMY_TYPE_COUNT) ? _killCounts[index] : 0;
}

GEEnemyManagerState GEEnemyManager::snapshotState() const {
    GEEnemyManagerState state;
    state.spawnTimer = _spawnTimer;
    state.spawnInterval = _spawnInterval;
    state.difficultyTimer = _difficultyTimer;
    state.elapsedTime = _elapsedTime;
    state.randomState = _randomState;
    for (int i = 0; i < Enemy::ENEMY_TYPE_COUNT; ++i)
        state.killCounts[static_cast<size_t>(i)] = _killCounts[i];

    for (unsigned int i = 0; i < _activeEnemies.size(); ++i) {
        GEEnemy* enemy = _activeEnemies[i];
        if (!enemy || !enemy->isAlive()) continue;
        GEEnemyState enemyState = enemy->snapshotState();
        state.addEnemyState(enemyState);
    }
    return state;
}

void GEEnemyManager::applyState(const GEEnemyManagerState& state) {
    _spawnTimer = state.spawnTimer;
    _spawnInterval = state.spawnInterval > 0.0f ? state.spawnInterval : Enemy::DEFAULT_SPAWN_INTERVAL;
    _difficultyTimer = state.difficultyTimer;
    _elapsedTime = state.elapsedTime;
    _randomState = state.randomState;

    resetKillCounts();
    for (int i = 0; i < Enemy::ENEMY_TYPE_COUNT; ++i)
        _killCounts[i] = state.killCounts[static_cast<size_t>(i)];

    _enemies.reset(Enemy::MAX_ENEMIES);

    const bool infinite = _mapData && _mapData->isInfiniteMap();
    const int mapW = _mapData ? _mapData->getActiveChunkPixelWidth() : 0;
    const int mapH = _mapData ? _mapData->getActiveChunkPixelHeight() : 0;

    for (unsigned int i = 0; i < state.enemyStates.size(); ++i) {
        const GEEnemyState& enemyState = state.enemyStates[i];
        if (enemyState.hp <= 0) continue;
        GEEnemy* enemy = new GEEnemy(
            enemyState.type,
            _resources.enemyTexture(enemyState.type));
        if (infinite)
            enemy->setMapBounds(-1, -1);
        else
            enemy->setMapBounds(mapW, mapH);
        enemy->applyState(enemyState);
        _enemies.add(enemy);
    }

    rebuildActiveEnemies();
}
