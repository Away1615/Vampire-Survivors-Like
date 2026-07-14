#include "GEProjectile.h"

void GEProjectile::activate(
    ProjectileOwner owner,
    float dirX,
    float dirY,
    float speed,
    int damage) {
    _owner = owner;
    _dirX = dirX;
    _dirY = dirY;
    _speed = speed;
    _damage = damage;
    _active = true;
}

void GEProjectile::update(float deltaTime) {
    if (!_active) return;
    transformComponent().translate(
        _dirX * _speed * deltaTime,
        _dirY * _speed * deltaTime);
}

GEProjectileState GEProjectile::snapshotState() const {
    GEProjectileState state;
    state.owner = _owner;
    state.centerX = getCenterX();
    state.centerY = getCenterY();
    state.dirX = _dirX;
    state.dirY = _dirY;
    state.speed = _speed;
    state.damage = _damage;
    return state;
}

void GEProjectile::applyState(const GEProjectileState& state) {
    activate(state.owner, state.dirX, state.dirY, state.speed, state.damage);
    setCenter(state.centerX, state.centerY);
}
