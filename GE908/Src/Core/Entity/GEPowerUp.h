#pragma once
#include "../../Foundation/GEModel.h"
#include "../../Foundation/GECollisible.h"
#include "../../Foundation/GEObjectPool.h"
#include "../State/GECodable.h"
#include "../State/GEGameState.h"

class GEPowerUp : public GECollisible, public GEPoolable, public GECodable<GEPowerUpState> {
private:
    bool _active = false;
    float _timeToLive = 0.0f;
    float _remainingTime = 0.0f;
    GEPowerUpType _type = GEPowerUpType::None;

public:
    explicit GEPowerUp(const Image& texture);
    void spawn(GEPowerUpType type, const Image& texture,
        float centerX, float centerY, float lifeTimeSeconds);
    void update(float deltaTime);
    void deactivate();
    bool isAlive() const { return _active; }
    bool isActiveElement() const override { return _active; }
    GEPowerUpType getType() const { return _type; }

    GEPowerUpState snapshotState() const override;
    void applyState(const GEPowerUpState& state) override;
};
