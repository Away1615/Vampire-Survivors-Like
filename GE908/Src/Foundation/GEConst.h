#pragma once


// Player related values
namespace Player {
    // player sprite
    static constexpr const char* playerSpriteFilePath = "Src/Assets/Textures/player.png";
    
    // fire damage interval when collide with tile_fire
    static constexpr float FIRE_DAMAGE_INTERVAL = 1.0f;

    // fire damage when collide with tile_fire
    static constexpr int FIRE_DAMAGE = 15;
    static constexpr float COLLISION_RADIUS = 16.0f;

    // max number of projectiles that send by player
    static constexpr auto MAX_PROJECTILES = 100;

    // speed of player projectile
    static constexpr auto PLAYER_PROJECTILE_SPEED = 300.0f;

    // 
    static constexpr auto PLAYER_PROJECTILE_DAMAGE = 30;
    static constexpr auto PLAYER_MAX_AOE_EFFECTS = 10;
    static constexpr auto PLAYER_MAX_AOE_TARGETS = 4;
    static constexpr auto PLAYER_BASE_AOE_TARGETS = 2;
    static constexpr auto PLAYER_AOE_DAMAGE = 100;
    static constexpr auto PLAYER_BASE_AUTO_ATTACK_INTERVAL = 2.0f;
    static constexpr auto PLAYER_BUFF_ATTACK_SPEED_MULTIPLIER = 4.0f;
    static constexpr auto PLAYER_POWERUP_DURATION_SECONDS = 5.0f;
    static constexpr auto PLAYER_HEAL_VALUE = 50;
}

// Enemy related values
namespace Enemy {

    // player get damage
    constexpr int PLAYER_COLLISION_DAMAGE = 20;

    // enemy get damage
    constexpr int ENEMY_COLLISION_DAMAGE = 30;
    static constexpr float NORMAL_COLLISION_RADIUS = 24.0f;
    static constexpr float FAST_COLLISION_RADIUS = 32.0f;
    static constexpr float HEAVY_COLLISION_RADIUS = 32.0f;
    static constexpr float STATIC_COLLISION_RADIUS = 26.0f;

    static constexpr auto MAX_ENEMIES = 2000;
    static constexpr float DEFAULT_SPAWN_INTERVAL = 3.0f;
    static constexpr float MIN_SPAWN_INTERVAL = 0.5f;
    static constexpr float SPAWN_INTERVAL_STEP = 0.25f;
    static constexpr float SPAWN_DIFFICULTY_STEP_TIME = 25.0f;

    static constexpr int BASE_ACTIVE_ENEMY_CAP = 12;
    static constexpr int ACTIVE_ENEMY_CAP_INCREMENT = 4;
    static constexpr float ACTIVE_ENEMY_CAP_STEP_TIME = 45.0f;

    static constexpr int ENEMY_TYPE_COUNT = 4;
}

// Map related values
namespace Map {
    constexpr auto TILES_COUNT = 25;
}

// PowerUp related values
namespace PowerUp {
    constexpr const char* ATTACK_SPEED_TEXTURE = "Src/Assets/Textures/attack_speed.png";
    constexpr const char* AOE_TARGET_TEXTURE = "Src/Assets/Textures/aoe_target.png";
    constexpr const char* HEAL_TEXTURE = "Src/Assets/Textures/heal.png";

    static constexpr float SPAWN_INTERVAL_SECONDS = 10.0f;
    static constexpr float POWERUP_LIFETIME_SECONDS = 18.0f;
    static constexpr float DROP_CHANCE = 0.5f;
    static constexpr float COLLISION_RADIUS = 16.0f;
}

// Projectile related values
namespace Projectile {
    constexpr const char* PLAYER_PROJECTILE_TEXTURE = "Src/Assets/Textures/arrow.png";
    constexpr const char* ENEMY_PROJECTILE_TEXTURE = "Src/Assets/Textures/enemy_bullet.png";

    constexpr const int MAX_PROJECTILES = 1000;
    constexpr float COLLISION_RADIUS = 16.0f;
}
