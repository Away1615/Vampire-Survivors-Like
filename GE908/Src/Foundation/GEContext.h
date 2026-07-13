#pragma once
#include "../../ThirdParty/GamesEngineeringBase.h"
#include "../Core/Interface/GEProvider.h"

using namespace GamesEngineeringBase;

struct GEContext {

private:
    Window& _window;
    EnemyProvider& _enemyProvider;
    MapProvider& _mapProvider;
    ProjectileProvider& _projectileProvider;
    PowerUpProvider& _powerUpProvider;
    PlayerProvider& _playerProvider;

public:
    GEContext(Window& window,
        EnemyProvider& enemyProvider,
        MapProvider& mapProvider,
        ProjectileProvider& projectileProvider,
        PowerUpProvider& powerupProvider,
        PlayerProvider& playerProvider)
        : _window(window),
        _enemyProvider(enemyProvider),
        _mapProvider(mapProvider),
        _projectileProvider(projectileProvider),
        _powerUpProvider(powerupProvider),
        _playerProvider(playerProvider)
    {
    }

    Window& window() const { return _window; }
    MapProvider& mapProvider() const { return _mapProvider; }
    ProjectileProvider& projectileProvider() const { return _projectileProvider; }
    PowerUpProvider& powerupProvider() const { return _powerUpProvider; }
    PlayerProvider& playerProvider() const { return _playerProvider; }
    EnemyProvider& enemyProvider() const { return _enemyProvider; }
};