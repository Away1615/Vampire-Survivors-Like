#pragma once
#include "../Entity/GEEnemy.h"
#include "../Map/GEMapData.h"
#include "../../Foundation/GEObjectPool.h"
#include "../../Foundation/GEUtility.h"
#include "../Interface/GEProvider.h"
#include <cstdint>
#include <vector>

class GEGameResources;

// Manages active enemies and defeat settlement.
class GEEnemyManager : public EnemyProvider {
private:
    const GEGameResources& _resources;
    GEObjectPool<GEEnemy*> _enemies;
    std::vector<GEEnemy*> _activeEnemies;

    float _spawnTimer = 0.0f;
    float _spawnInterval = Enemy::DEFAULT_SPAWN_INTERVAL;
    float _difficultyTimer = 0.0f;
    float _elapsedTime = 0.0f;
    uint32_t _randomState = 1u;

    int _killCounts[Enemy::ENEMY_TYPE_COUNT] = { 0 };
    GEMapData* _mapData = nullptr;

    bool spawnEnemyOutsideCamera(PlayerProvider& player);
    void rebuildActiveEnemies();
    void recordEnemyKill(GEEnemyType type);

public:
    explicit GEEnemyManager(const GEGameResources& resources);
    ~GEEnemyManager();

    int getEnemyCount() const { return static_cast<int>(_activeEnemies.size()); }
    GEEnemy* getEnemyAt(int index) const {
        return index >= 0 && index < getEnemyCount() ? _activeEnemies[index] : nullptr;
    }

    void load(GEMapData* mapData);
    void reset() override;
    void update(float deltaTime,
        PlayerProvider& playerProvider,
        ProjectileProvider& projectileProvider,
        PowerUpProvider& powerUpProvider) override;
    void draw(Window& window, const GECamera& camera);

    bool settleEnemyDefeat(GEEnemy& enemy, PowerUpProvider& powerUpProvider) override;
    void resetKillCounts();
    int getKillCount(GEEnemyType type) const;

    static bool isWaterTile(int tileID) { return tileID >= 14 && tileID <= 22; }

    GEEnemyManagerState snapshotState() const override;
    void applyState(const GEEnemyManagerState& state) override;
};
