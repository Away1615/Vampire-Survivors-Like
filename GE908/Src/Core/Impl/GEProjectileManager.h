#pragma once
#include "../Interface/GEProvider.h"
#include "../Entity/GEPlayer.h"
#include "../Entity/GEEnemy.h"
#include "../Entity/GEProjectile.h"
#include "../../Foundation/GEObjectPool.h"
#include "../../Foundation/GECollisible.h"
#include "../../Foundation/GEModel.h"

class GEProjectileManager : public ProjectileProvider {

private:
    GEObjectPool<GEProjectile*> _projectiles;

public:
    GEProjectileManager();
    ~GEProjectileManager();

    void load(GEMapData* mapData);
    void reset() override;

    void addProjectile(ProjectileOwner from, float startPointX, float startPointY,
        float dirX, float dirY, float speed, int damage);

    void update(float deltaTime, GEContext& ctx);
    void draw(Window& window, const GECamera& camera);

    GEProjectileManagerState snapshotState() const override;
    void applyState(const GEProjectileManagerState& state) override;
};
