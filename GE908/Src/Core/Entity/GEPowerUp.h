#pragma once
#include "../../Foundation/GEModel.h"
#include "../../Foundation/GECollisible.h"
#include "../../Foundation/GEObjectPool.h"
#include "../State/GECodable.h"
#include "../State/GEGameState.h"
#include "../Component/GEPowerUpComponent.h"

class GEPowerUp : public GECollisible, public GEPoolable, public GECodable<GEPowerUpState> {
private:
    GEPowerUpComponent _powerUp;

public:
    GEPowerUp();
    void spawn(GEPowerUpType type, float centerX, float centerY, float lifeTimeSeconds);
    void deactivate() { _powerUp.deactivate(); }
    bool isAlive() const { return _powerUp.isActive(); }
    bool isActiveElement() const override { return _powerUp.isActive(); }
    GEPowerUpType getType() const { return _powerUp.getType(); }

    GEPowerUpComponent& powerUpComponent() { return _powerUp; }
    const GEPowerUpComponent& powerUpComponent() const { return _powerUp; }

    GEPowerUpState snapshotState() const override;
    void applyState(const GEPowerUpState& state) override;
};
