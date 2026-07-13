#pragma once

#include "../Component/GEEnemyComponent.h"
#include "../../Foundation/Component/GETransformComponent.h"
#include "../Interface/GEProvider.h"
#include <cmath>

class GEEnemyRangedAttackSystem {
public:
    static void update(
        GEEnemyComponent& enemy,
        const GETransformComponent& transform,
        float targetX,
        float targetY,
        float deltaTime,
        ProjectileProvider& projectileProvider) {
        if (!enemy.isStationary()) return;

        const float attackCooldown = enemy.getAttackCooldown() + deltaTime;
        enemy.setAttackCooldown(attackCooldown);
        if (attackCooldown < enemy.getAttackRate()) return;
        enemy.setAttackCooldown(0.0f);

        const float centerX = transform.getCenterX();
        const float centerY = transform.getCenterY();
        float dirX = targetX - centerX;
        float dirY = targetY - centerY;
        const float length = std::sqrt(dirX * dirX + dirY * dirY);
        if (length == 0.0f) return;

        dirX /= length;
        dirY /= length;
        projectileProvider.addProjectile(
            ProjectileOwner::FromEnemy,
            centerX,
            centerY,
            dirX,
            dirY,
            100.0f,
            100);
    }
};
