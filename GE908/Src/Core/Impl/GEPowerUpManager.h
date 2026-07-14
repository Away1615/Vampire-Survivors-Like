#pragma once
#include "../../../ThirdParty/GamesEngineeringBase.h"
#include "../Map/GEMapData.h"
#include "../../Foundation/GECamera.h"
#include "../../Foundation/GEObjectPool.h"
#include "../Entity/GEPowerUp.h"
#include "../Interface/GEProvider.h"
#include <cstdint>

using namespace GamesEngineeringBase;

class GEGameResources;

// Manages power-up drops and collection.
class GEPowerUpManager : public PowerUpProvider {
private:
    const GEGameResources& _resources;
    GEObjectPool<GEPowerUp*> _powerUps;
    GEMapData* _mapData = nullptr;
    uint32_t _randomState = 1u;

    void spawnPowerUpAt(const GEPoint& point);

public:
    explicit GEPowerUpManager(const GEGameResources& resources);
    ~GEPowerUpManager();

    void load(GEMapData* mapData);
    void reset() override;
    void update(float deltaTime, PlayerProvider& playerProvider) override;
    void draw(Window& window, const GECamera& camera);
    void onEnemyDefeated(const GEPoint& position) override;

    GEPowerUpManagerState snapshotState() const override;
    void applyState(const GEPowerUpManagerState& state) override;
};
