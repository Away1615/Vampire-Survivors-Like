#pragma once

#include "../Component/GEProjectileComponent.h"
#include "../../Foundation/Component/GETransformComponent.h"

class GEProjectileMovementSystem {
public:
    static void update(
        GETransformComponent& transform,
        const GEProjectileComponent& projectile,
        float deltaTime) {
        if (!projectile.isActive()) return;

        transform.translate(
            projectile.getDirectionX() * projectile.getSpeed() * deltaTime,
            projectile.getDirectionY() * projectile.getSpeed() * deltaTime);
    }
};
