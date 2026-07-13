#include "GEPowerUp.h"

GEPowerUp::GEPowerUp()
    : GECollisible(PowerUp::ATTACK_SPEED_TEXTURE, GECollisionLayer::PowerUp) {
    setCircleCollider(PowerUp::COLLISION_RADIUS);
}

void GEPowerUp::spawn(GEPowerUpType type, float centerX, float centerY, float lifeTimeSeconds) {
    const bool typeChanged = (getType() != type);
    _powerUp.spawn(type, lifeTimeSeconds);

    switch (getType()) {
    case GEPowerUpType::AttackSpeedBoost:
        if (typeChanged) loadSprite(PowerUp::ATTACK_SPEED_TEXTURE);
        break;
    case GEPowerUpType::AdditionalAoeTarget:
        if (typeChanged) loadSprite(PowerUp::AOE_TARGET_TEXTURE);
        break;
    case GEPowerUpType::HealPlayer:
        if (typeChanged) loadSprite(PowerUp::HEAL_TEXTURE);
        break;
    default:
        break;
    }

    setCenter(centerX, centerY);
}

GEPowerUpState GEPowerUp::snapshotState() const {
    GEPowerUpState state;
    state.type = _powerUp.getType();
    state.centerX = getCenterX();
    state.centerY = getCenterY();
    state.timeToLive = _powerUp.getTimeToLive();
    state.remainingTime = _powerUp.getRemainingTime();
    if (_powerUp.isActive()) state.activate();
    else state.deactivate();
    return state;
}

void GEPowerUp::applyState(const GEPowerUpState& state) {
    _powerUp.restore(state.type, state.remainingTime, state.timeToLive, state.isActiveElement());

    switch (_powerUp.getType()) {
    case GEPowerUpType::AttackSpeedBoost:
        loadSprite(PowerUp::ATTACK_SPEED_TEXTURE);
        break;
    case GEPowerUpType::AdditionalAoeTarget:
        loadSprite(PowerUp::AOE_TARGET_TEXTURE);
        break;
    case GEPowerUpType::HealPlayer:
        loadSprite(PowerUp::HEAL_TEXTURE);
        break;
    default:
        break;
    }

    setCenter(state.centerX, state.centerY);
}
