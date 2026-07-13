#pragma once
#include "../Entity/GEEnemy.h"
#include "../Entity/GEPlayer.h"
#include "../Map/GEMapData.h"
#include "../../Foundation/GEObjectPool.h"
#include "../../Foundation/GEUtility.h"
#include "../Interface/GEProvider.h"

class GEEnemyManager : public EnemyProvider {
private:
    GEObjectPool<GEEnemy*> _enemies;

    int _activeEnemyCount = 0;
    float _spawnTimer = 0.0f;
    float _spawnInterval = Enemy::DEFAULT_SPAWN_INTERVAL;
    float _difficultyTimer = 0.0f;
    float _elapsedTime = 0.0f;

    int _killCounts[Enemy::ENEMY_TYPE_COUNT] = { 0 };
    GEMapData* _mapData = nullptr;

    bool spawnEnemyOutsideCamera(PlayerProvider& player);
    void removeEnemy(GEEnemy* enemy);

public:
    GEEnemyManager();
    ~GEEnemyManager();

    int getEnemyCount() const { return _activeEnemyCount; }
    GEEnemy* getEnemyAt(int index) const { return _enemies[index]; }

    void load(GEMapData* mapData);
    void reset() override;
    void update(float deltaTime, GEContext& ctx);
    void draw(Window& window, const GECamera& camera);

    void registerEnemyKill(GEEnemyType type);
    void resetKillCounts();
    int getKillCount(GEEnemyType type) const;

    static bool isWaterTile(int tileID) { return tileID >= 14 && tileID <= 22; }

    GEEnemyManagerState snapshotState() const override;
    void applyState(const GEEnemyManagerState& state) override;
};
