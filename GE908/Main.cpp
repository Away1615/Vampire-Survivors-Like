#include "Src/GameManager.h"
#include "Src/Core/Impl/GEPowerUpManager.h"
#include "Src/Core/Impl/GEProjectileManager.h"
#include "Src/Core/Impl/GEEnemyManager.h"
#include "Src/Core/Impl/GEMapsManager.h"
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
    GEPlayer player;
    GEEnemyManager enemyManager; 
    GEProjectileManager projectileManager;
    GEPowerUpManager powerUpManager;
    GEMapsManager maps;
    GameManager game(window, maps, player, enemyManager, projectileManager, powerUpManager);
    game.run();
    return 0;
}
