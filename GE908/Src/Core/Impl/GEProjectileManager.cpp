#include "GEProjectileManager.h"
#include "../Entity/GEEnemy.h"
#include "../Resource/GEGameResources.h"

GEProjectileManager::GEProjectileManager(const GEGameResources& resources)
    : _resources(resources) {
    _projectiles.reset(Projectile::MAX_PROJECTILES);
}

GEProjectileManager::~GEProjectileManager() = default;

void GEProjectileManager::reset() {
    _projectiles.reset(Projectile::MAX_PROJECTILES);
}

void GEProjectileManager::addProjectile(ProjectileOwner from,
    float startPointX, float startPointY,
    float dirX, float dirY, float speed, int damage)
{
    const Image& texture = _resources.projectileTexture(from);

    // Reuse inactive projectiles.
    for (unsigned int i = 0; i < _projectiles.size(); ++i) {
        GEProjectile* p = _projectiles.getAt(i);
        if (p == nullptr) {
            _projectiles.add(new GEProjectile(texture, from,
                startPointX, startPointY, dirX, dirY, speed, damage));
            return;
        }
        if (!p->isActiveElement()) {
            p->spawn(texture, from,
                startPointX, startPointY, dirX, dirY, speed, damage);
            return;
        }
    }
}

void GEProjectileManager::update(float deltaTime,
    PlayerProvider& player,
    EnemyProvider& enemyProvider,
    PowerUpProvider& powerUpProvider,
    const GECamera& camera) {
    const int enemyCount = enemyProvider.getEnemyCount();
    const float minX = camera.getX() - Projectile::DESPAWN_MARGIN;
    const float minY = camera.getY() - Projectile::DESPAWN_MARGIN;
    const float maxX = camera.getX() + camera.getWidth() + Projectile::DESPAWN_MARGIN;
    const float maxY = camera.getY() + camera.getHeight() + Projectile::DESPAWN_MARGIN;

    for (unsigned int i = 0; i < _projectiles.size(); ++i) {
        GEProjectile* projectile = _projectiles.getAt(i);
        if (!projectile || !projectile->isActiveElement()) continue;

        projectile->update(deltaTime);
        if (projectile->getCenterX() < minX
            || projectile->getCenterX() > maxX
            || projectile->getCenterY() < minY
            || projectile->getCenterY() > maxY) {
            projectile->deactivate();
            continue;
        }

        if (projectile->getOwner() == ProjectileOwner::FromPlayer) {
            for (int j = 0; j < enemyCount; ++j) {
                GEEnemy* enemy = enemyProvider.getEnemyAt(j);
                if (enemy && enemy->isAlive() && projectile->collide(*enemy)) {
                    enemy->takeDamage(projectile->getDamage());
                    if (!enemy->isAlive()) {
                        enemyProvider.settleEnemyDefeat(
                            *enemy,
                            powerUpProvider);
                    }
                    projectile->deactivate();
                    break;
                }
            }
        }
        else {
            if (projectile->collide(player.collisionBody())) {
                player.takeDamage(projectile->getDamage());
                projectile->deactivate();
            }
        }
    }

}

void GEProjectileManager::draw(Window& window, const GECamera& camera) {
    for (unsigned int i = 0; i < _projectiles.size(); ++i) {
        GEProjectile* projectile = _projectiles.getAt(i);
        if (projectile && projectile->isActiveElement())
            projectile->draw(window, camera);
    }
}

GEProjectileManagerState GEProjectileManager::snapshotState() const {
    GEProjectileManagerState state;
    for (unsigned int i = 0; i < _projectiles.size(); ++i) {
        GEProjectile* projectile = _projectiles.getAt(i);
        if (!projectile || !projectile->isActiveElement()) continue;
        state.addProjectileState(projectile->snapshotState());
    }
    return state;
}

void GEProjectileManager::applyState(const GEProjectileManagerState& state) {
    _projectiles.reset(Projectile::MAX_PROJECTILES);

    for (unsigned int i = 0; i < state.projectiles.size(); ++i) {
        const GEProjectileState& projectileState = state.projectiles[i];
        GEProjectile* projectile = new GEProjectile(
            _resources.projectileTexture(projectileState.owner));
        projectile->applyState(projectileState);
        _projectiles.add(projectile);
    }
}
