#pragma once

#include "../../Foundation/GECamera.h"
#include "../../Foundation/GEFont.h"
#include "../Interface/GEProvider.h"
#include "../Map/GEMapData.h"
#include "../Save/GESaveTypes.h"

// Coordinates world state for one gameplay session.
class GEGameSession {
private:
    Window& _window;
    const GEFont& _font;
    MapProvider& _mapProvider;
    PlayerProvider& _player;
    EnemyProvider& _enemyProvider;
    ProjectileProvider& _projectileProvider;
    PowerUpProvider& _powerUpProvider;
    GECamera _camera;
    GEMapData _mapData;

    GEMapMode _mapMode = GEMapMode::Fixed;
    float _levelTimeRemaining = 120.0f;
    bool _isActive = false;
    bool _isDefeated = false;
    bool _isVictory = false;

    bool prepareWorld(GEMapMode mapMode);
    void drawHud();

public:
    GEGameSession(Window& window,
        const GEFont& font,
        MapProvider& mapProvider,
        PlayerProvider& playerProvider,
        EnemyProvider& enemyProvider,
        ProjectileProvider& projectileProvider,
        PowerUpProvider& powerUpProvider);

    bool start(GEMapMode mapMode);
    void reset();
    void update(float deltaTime);
    void render();

    GEGameSnapshot snapshot() const;
    bool restore(const GEGameSnapshot& snapshot);

    bool isActive() const { return _isActive; }
    bool isDefeated() const { return _isDefeated; }
    bool isVictory() const { return _isVictory; }

    GEGameSession(const GEGameSession&) = delete;
    GEGameSession& operator=(const GEGameSession&) = delete;
};
