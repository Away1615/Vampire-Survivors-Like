#pragma once

#include "../Component/GEPowerUpComponent.h"

class GEPowerUpLifetimeSystem {
public:
    static void update(GEPowerUpComponent& powerUp, float deltaTime) {
        if (!powerUp.isActive() || powerUp.getTimeToLive() <= 0.0f) return;

        const float remainingTime = powerUp.getRemainingTime() - deltaTime;
        if (remainingTime <= 0.0f) {
            powerUp.deactivate();
        }
        else {
            powerUp.setRemainingTime(remainingTime);
        }
    }
};
