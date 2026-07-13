#pragma once

#include "../Component/GEPlayerComponent.h"
#include "../Entity/GEEnemy.h"
#include "../Interface/GEProvider.h"
#include "../../Foundation/Component/GETransformComponent.h"
#include <cfloat>
#include <cmath>

class GEPlayerCombatSystem {
private:
    static void updateAutoAttack(
        GEPlayerComponent& player,
        const GETransformComponent& transform,
        float deltaTime,
        EnemyProvider* enemyProvider,
        ProjectileProvider* projectileProvider) {
        if (!enemyProvider || !projectileProvider) return;
        if (!player.advanceAutoAttack(deltaTime)) return;

        GEEnemy* nearest = nullptr;
        float nearestDistSq = FLT_MAX;

        const int enemyCount = enemyProvider->getEnemyCount();
        for (int i = 0; i < enemyCount; ++i) {
            GEEnemy* enemy = enemyProvider->getEnemyAt(i);
            if (!enemy || !enemy->isAlive()) continue;

            const float dx = enemy->getCenterX() - transform.getCenterX();
            const float dy = enemy->getCenterY() - transform.getCenterY();
            const float distanceSquared = dx * dx + dy * dy;
            if (distanceSquared < nearestDistSq) {
                nearestDistSq = distanceSquared;
                nearest = enemy;
            }
        }

        if (!nearest) return;

        const float centerX = transform.getCenterX();
        const float centerY = transform.getCenterY();
        float dirX = nearest->getCenterX() - centerX;
        float dirY = nearest->getCenterY() - centerY;
        const float length = std::sqrt(dirX * dirX + dirY * dirY);
        if (length == 0.0f) return;

        dirX /= length;
        dirY /= length;
        projectileProvider->addProjectile(
            ProjectileOwner::FromPlayer,
            centerX,
            centerY,
            dirX,
            dirY,
            Player::PLAYER_PROJECTILE_SPEED,
            Player::PLAYER_PROJECTILE_DAMAGE);
    }

    static void executeAoe(
        GEPlayerComponent& player,
        const GETransformComponent& transform,
        EnemyProvider* enemyProvider,
        PowerUpProvider* powerUpProvider) {
        if (!enemyProvider) return;

        const float originX = transform.getCenterX();
        const float originY = transform.getCenterY();
        GEEnemy* topEnemies[Player::PLAYER_MAX_AOE_TARGETS];
        int topEnemyHp[Player::PLAYER_MAX_AOE_TARGETS];
        int candidateCount = 0;

        const int enemyCount = enemyProvider->getEnemyCount();
        const float radiusSquared = player.getAoeRadius() * player.getAoeRadius();
        for (int i = 0; i < enemyCount; ++i) {
            GEEnemy* enemy = enemyProvider->getEnemyAt(i);
            if (!enemy || !enemy->isAlive()) continue;

            const float dx = enemy->getCenterX() - originX;
            const float dy = enemy->getCenterY() - originY;
            if (dx * dx + dy * dy > radiusSquared) continue;

            const int hp = enemy->getHP();
            int insertPosition = 0;
            while (insertPosition < candidateCount && hp <= topEnemyHp[insertPosition]) {
                ++insertPosition;
            }

            if (candidateCount < Player::PLAYER_MAX_AOE_TARGETS) {
                for (int j = candidateCount; j > insertPosition; --j) {
                    topEnemies[j] = topEnemies[j - 1];
                    topEnemyHp[j] = topEnemyHp[j - 1];
                }
                topEnemies[insertPosition] = enemy;
                topEnemyHp[insertPosition] = hp;
                ++candidateCount;
            }
            else if (insertPosition < Player::PLAYER_MAX_AOE_TARGETS) {
                for (int j = Player::PLAYER_MAX_AOE_TARGETS - 1; j > insertPosition; --j) {
                    topEnemies[j] = topEnemies[j - 1];
                    topEnemyHp[j] = topEnemyHp[j - 1];
                }
                topEnemies[insertPosition] = enemy;
                topEnemyHp[insertPosition] = hp;
            }
        }

        if (candidateCount == 0) return;

        const int targetsToHit = candidateCount < player.getAoeTargetCount()
            ? candidateCount
            : player.getAoeTargetCount();
        for (int i = 0; i < targetsToHit; ++i) {
            GEEnemy& enemy = *topEnemies[i];
            enemy.takeDamage(Player::PLAYER_AOE_DAMAGE);
            if (!enemy.isAlive()) {
                enemyProvider->registerEnemyKill(enemy.getType());
                if (powerUpProvider) {
                    powerUpProvider->onEnemyDefeated(GEPoint(enemy.getCenterX(), enemy.getCenterY()));
                }
            }
            player.recordAoeImpact(enemy.getCenterX(), enemy.getCenterY());
        }
        player.startAoeCooldown();
    }

    static void updateAoe(
        GEPlayerComponent& player,
        const GETransformComponent& transform,
        float deltaTime,
        EnemyProvider* enemyProvider,
        PowerUpProvider* powerUpProvider) {
        player.updateAoe(deltaTime);
        if (player.isAoeRequested() && player.getAoeCooldownTimer() <= 0.0f) {
            executeAoe(player, transform, enemyProvider, powerUpProvider);
        }
        player.clearAoeRequest();
    }

public:
    static void update(
        GEPlayerComponent& player,
        const GETransformComponent& transform,
        float deltaTime,
        EnemyProvider* enemyProvider,
        ProjectileProvider* projectileProvider,
        PowerUpProvider* powerUpProvider) {
        updateAutoAttack(player, transform, deltaTime, enemyProvider, projectileProvider);
        updateAoe(player, transform, deltaTime, enemyProvider, powerUpProvider);
    }
};
