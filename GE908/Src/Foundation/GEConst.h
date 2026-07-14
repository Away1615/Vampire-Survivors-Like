#pragma once


// Player tuning.
namespace Player {
    static constexpr float FIRE_DAMAGE_INTERVAL = 1.0f;

    static constexpr int FIRE_DAMAGE = 15;
    static constexpr float COLLISION_RADIUS = 16.0f;
    static constexpr int MOVE_SPEED = 340;

    static constexpr auto PLAYER_PROJECTILE_DAMAGE = 30;
    static constexpr auto PLAYER_MAX_AOE_EFFECTS = 10;
    static constexpr auto PLAYER_MAX_AOE_TARGETS = 4;
    static constexpr auto PLAYER_BASE_AOE_TARGETS = 2;
    static constexpr auto PLAYER_AOE_DAMAGE = 100;
    static constexpr auto PLAYER_BASE_AUTO_ATTACK_INTERVAL = 1.0f;
    static constexpr auto PLAYER_BUFF_ATTACK_SPEED_MULTIPLIER = 4.0f;
    static constexpr auto PLAYER_POWERUP_DURATION_SECONDS = 5.0f;
    static constexpr auto PLAYER_HEAL_VALUE = 50;
    static constexpr int WALK_FRAME_COUNT = 8;
    static constexpr float WALK_FRAMES_PER_SECOND = 8.0f;
    static constexpr float FRONT_BACK_WALK_DRAW_OFFSET_X = 11.0f;
    static constexpr float SIDE_WALK_DRAW_OFFSET_X = 8.0f;
    static constexpr float WALK_DRAW_OFFSET_Y = -2.0f;
}

// Enemy tuning.
namespace Enemy {

    constexpr int PLAYER_COLLISION_DAMAGE = 20;

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
    static constexpr float RANGED_ATTACK_INTERVAL = 3.0f;

    static constexpr int BASE_ACTIVE_ENEMY_CAP = 12;
    static constexpr int ACTIVE_ENEMY_CAP_INCREMENT = 4;
    static constexpr float ACTIVE_ENEMY_CAP_STEP_TIME = 45.0f;

    static constexpr int ENEMY_TYPE_COUNT = 4;
}

// Map tuning.
namespace Map {
    constexpr auto TILES_COUNT = 25;
}

// Power-up tuning.
namespace PowerUp {
    static constexpr float POWERUP_LIFETIME_SECONDS = 18.0f;
    static constexpr float DROP_CHANCE = 0.5f;
    static constexpr float COLLISION_RADIUS = 16.0f;
    static constexpr int MAX_POWERUPS = 100;
}

// Projectile tuning.
namespace Projectile {
    constexpr const int MAX_PROJECTILES = 1000;
    constexpr float MOVE_SPEED = 360.0f;
    constexpr float COLLISION_RADIUS = 16.0f;
    constexpr float DESPAWN_MARGIN = 128.0f;
}
