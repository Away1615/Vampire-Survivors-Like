#include "Src/GameManager.h"
#include "Src/Core/Impl/GEPowerUpManager.h"
#include "Src/Core/Impl/GEProjectileManager.h"
#include "Src/Core/Impl/GEEnemyManager.h"
#include "Src/Core/Impl/GEMapsManager.h"
#include "Src/Core/Resource/GEGameResources.h"
#include "Src/Core/Session/GEGameSession.h"
#include "Src/Core/Entity/GEPlayer.h"
#include "Src/Foundation/GEPath.h"
#include <iostream>

int main() {
    if (!GEPath::initializeContentRoot()) {
        std::cerr << "Unable to locate the Src/Assets directory.\n";
        return 1;
    }

    Window window;
    window.create(854, 480, "WM908", false);

    GEGameResources resources;
    if (!resources.load()) {
        std::cerr << "Unable to load game resources.\n";
        return 1;
    }

    // Compose long-lived game services.
    GEPlayer player(
        resources.playerFrontWalkTexture(),
        resources.playerBackWalkTexture(),
        resources.playerSideWalkTexture());
    GEEnemyManager enemyManager(resources);
    GEProjectileManager projectileManager(resources);
    GEPowerUpManager powerUpManager(resources);
    GEMapsManager maps(resources);
    GEGameSession session(window, resources.font(), maps, player,
        enemyManager, projectileManager, powerUpManager);
    GameManager game(window, resources.font(), session);
    game.run();
    return 0;
}
