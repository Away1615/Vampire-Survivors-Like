#pragma once
#include "../../../ThirdParty/GamesEngineeringBase.h"
#include "../Map/GEMapData.h"
#include "../../Foundation/GECamera.h"
#include "../../Foundation/GEObjectPool.h"
#include "../Entity/GEPowerUp.h"
#include "../Interface/GEProvider.h"
#include "../../Foundation/GEContext.h"

using namespace GamesEngineeringBase;

class GEPowerUpManager : public PowerUpProvider {
private:
    GEObjectPool<GEPowerUp*> _powerUps;
    GEMapData* _mapData = nullptr;
    float _spawnTimer = 0.0f;

    void spawnPowerUpAt(const GEPoint& point);

public:
    GEPowerUpManager();
    ~GEPowerUpManager();

    void load(GEMapData* mapData);
    void reset() override;
    void update(float deltaTime, GEContext& ctx);
    void draw(Window& window, const GECamera& camera);
    void onEnemyDefeated(const GEPoint& position) override;

    GEPowerUpManagerState snapshotState() const override;
    void applyState(const GEPowerUpManagerState& state) override;
};
