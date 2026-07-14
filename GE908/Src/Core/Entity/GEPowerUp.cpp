#include "GEPowerUp.h"

GEPowerUp::GEPowerUp(const Image& texture)
    : GECollisible(texture, GECollisionLayer::PowerUp) {
    setCircleCollider(PowerUp::COLLISION_RADIUS);
}

void GEPowerUp::spawn(GEPowerUpType type, const Image& texture,
    float centerX, float centerY, float lifeTimeSeconds) {
    const bool typeChanged = (getType() != type);
    _type = type;
    _timeToLive = lifeTimeSeconds;
    _remainingTime = lifeTimeSeconds;
    _active = true;

    if (typeChanged) setSprite(texture);

    setCenter(centerX, centerY);
}

void GEPowerUp::update(float deltaTime) {
    if (!_active || _timeToLive <= 0.0f) return;

    _remainingTime -= deltaTime;
    if (_remainingTime <= 0.0f) deactivate();
}

void GEPowerUp::deactivate() {
    _active = false;
    _remainingTime = 0.0f;
}

GEPowerUpState GEPowerUp::snapshotState() const {
    GEPowerUpState state;
    state.type = _type;
    state.centerX = getCenterX();
    state.centerY = getCenterY();
    state.timeToLive = _timeToLive;
    state.remainingTime = _remainingTime;
    return state;
}

void GEPowerUp::applyState(const GEPowerUpState& state) {
    _type = state.type;
    _remainingTime = state.remainingTime;
    _timeToLive = state.timeToLive;
    _active = true;
    setCenter(state.centerX, state.centerY);
}
