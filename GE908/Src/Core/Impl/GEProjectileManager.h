#pragma once
#include "../Interface/GEProvider.h"
#include "../Entity/GEProjectile.h"
#include "../../Foundation/GEObjectPool.h"
#include "../../Foundation/GECollisible.h"
#include "../../Foundation/GEModel.h"

class GEGameResources;

// Manages projectile lifetime and collisions.
class GEProjectileManager : public ProjectileProvider {

private:
    const GEGameResources& _resources;
    GEObjectPool<GEProjectile*> _projectiles;

public:
    explicit GEProjectileManager(const GEGameResources& resources);
    ~GEProjectileManager();

    void reset() override;

    void addProjectile(ProjectileOwner from, float startPointX, float startPointY,
        float dirX, float dirY, float speed, int damage);

    void update(float deltaTime,
        PlayerProvider& playerProvider,
        EnemyProvider& enemyProvider,
        PowerUpProvider& powerUpProvider,
        const GECamera& camera) override;
    void draw(Window& window, const GECamera& camera);

    GEProjectileManagerState snapshotState() const override;
    void applyState(const GEProjectileManagerState& state) override;
};
