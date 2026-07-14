#pragma once
#include "../../Foundation/GEObjectPool.h"
#include "../../Foundation/GECollisible.h"
#include "../../Foundation/GEModel.h"
#include "../../Foundation/GEConst.h"
#include "../State/GECodable.h"
#include "../State/GEGameState.h"

class GEProjectile : public GECollisible, public GEPoolable, public GECodable<GEProjectileState> {
private:
    float _speed = 0.0f;
    float _dirX = 0.0f;
    float _dirY = 0.0f;
    int _damage = 0;
    bool _active = false;
    ProjectileOwner _owner = ProjectileOwner::FromPlayer;

    void activate(ProjectileOwner owner, float dirX, float dirY, float speed, int damage);

public:
    explicit GEProjectile(const Image& texture)
        : GECollisible(texture, GECollisionLayer::Projectile) {
        setCircleCollider(Projectile::COLLISION_RADIUS);
    }

    GEProjectile(const Image& texture, ProjectileOwner owner,
        float centerX, float centerY, float dirX, float dirY,
        float speed, int damage)
        : GECollisible(texture, GECollisionLayer::Projectile) {
        setCircleCollider(Projectile::COLLISION_RADIUS);
        activate(owner, dirX, dirY, speed, damage);
        setCenter(centerX, centerY);
    }

    bool isActiveElement() const override { return _active; }
    void deactivate() { _active = false; }

    ProjectileOwner getOwner() const { return _owner; }
    int getDamage() const { return _damage; }

    void spawn(const Image& texture, ProjectileOwner owner,
        float centerX, float centerY, float dirX, float dirY,
        float speed, int damage) {
        if (_owner != owner) {
            setSprite(texture);
        }
        activate(owner, dirX, dirY, speed, damage);
        setCenter(centerX, centerY);
    }

    void update(float deltaTime);

    GEProjectileState snapshotState() const override;
    void applyState(const GEProjectileState& state) override;
};
