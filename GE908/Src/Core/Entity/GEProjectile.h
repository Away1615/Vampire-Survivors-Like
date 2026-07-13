#pragma once
#include "../../Foundation/GEObjectPool.h"
#include "../../Foundation/GECollisible.h"
#include "../../Foundation/GEModel.h"
#include "../../Foundation/GEConst.h"
#include "../State/GECodable.h"
#include "../State/GEGameState.h"
#include "../Component/GEProjectileComponent.h"

class GEProjectile : public GECollisible, public GEPoolable, public GECodable<GEProjectileState> {
private:
    GEProjectileComponent _projectile;

public:
    GEProjectile() : GECollisible(Projectile::PLAYER_PROJECTILE_TEXTURE, GECollisionLayer::Projectile) {
        setCircleCollider(Projectile::COLLISION_RADIUS);
    }

    GEProjectile(const std::string& texturePath, ProjectileOwner owner,
        float centerX, float centerY, float dirX, float dirY,
        float speed, int damage)
        : GECollisible(texturePath, GECollisionLayer::Projectile) {
        setCircleCollider(Projectile::COLLISION_RADIUS);
        _projectile.spawn(owner, dirX, dirY, speed, damage);
        setCenter(centerX, centerY);
    }

    bool isActiveElement() const override { return _projectile.isActive(); }
    void deactivate() { _projectile.deactivate(); }

    ProjectileOwner getOwner() const { return _projectile.getOwner(); }
    int getDamage() const { return _projectile.getDamage(); }

    void spawn(const std::string& texturePath, ProjectileOwner owner,
        float centerX, float centerY, float dirX, float dirY,
        float speed, int damage) {
        if (_projectile.getOwner() != owner) {
            loadSprite(texturePath);
        }
        _projectile.spawn(owner, dirX, dirY, speed, damage);
        setCenter(centerX, centerY);
    }

    GEProjectileComponent& projectileComponent() { return _projectile; }
    const GEProjectileComponent& projectileComponent() const { return _projectile; }

    GEProjectileState snapshotState() const override;
    void applyState(const GEProjectileState& state) override;
};
