#include "GEProjectile.h"

GEProjectileState GEProjectile::snapshotState() const {
    GEProjectileState state;
    state.owner = _projectile.getOwner();
    state.centerX = getCenterX();
    state.centerY = getCenterY();
    state.dirX = _projectile.getDirectionX();
    state.dirY = _projectile.getDirectionY();
    state.speed = _projectile.getSpeed();
    state.damage = _projectile.getDamage();
    state.activate();
    return state;
}

void GEProjectile::applyState(const GEProjectileState& state) {
    _projectile.spawn(state.owner, state.dirX, state.dirY, state.speed, state.damage);

    const char* texturePath = (state.owner == ProjectileOwner::FromPlayer)
        ? Projectile::PLAYER_PROJECTILE_TEXTURE
        : Projectile::ENEMY_PROJECTILE_TEXTURE;
    loadSprite(texturePath);
    setCenter(state.centerX, state.centerY);
}
