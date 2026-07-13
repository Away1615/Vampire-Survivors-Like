#pragma once

#include "../../Foundation/GEModel.h"

class GEPowerUpComponent {
private:
    bool _active = false;
    float _timeToLive = 0.0f;
    float _remainingTime = 0.0f;
    GEPowerUpType _type = GEPowerUpType::None;

public:
    void spawn(GEPowerUpType type, float lifeTimeSeconds) {
        _type = type;
        _timeToLive = lifeTimeSeconds;
        _remainingTime = lifeTimeSeconds;
        _active = true;
    }

    void restore(GEPowerUpType type, float remainingTime, float timeToLive, bool active) {
        _type = type;
        _remainingTime = remainingTime;
        _timeToLive = timeToLive;
        _active = active;
    }

    void deactivate() {
        _active = false;
        _remainingTime = 0.0f;
    }

    bool isActive() const { return _active; }
    GEPowerUpType getType() const { return _type; }
    float getTimeToLive() const { return _timeToLive; }
    float getRemainingTime() const { return _remainingTime; }
    void setRemainingTime(float remainingTime) { _remainingTime = remainingTime; }
};
