#include "GEProjectileManager.h"
#include "../System/GEProjectileMovementSystem.h"

GEProjectileManager::GEProjectileManager() {
    _projectiles.resize(Projectile::MAX_PROJECTILES);
    _projectiles.fillNull(Projectile::MAX_PROJECTILES);
}

GEProjectileManager::~GEProjectileManager() {
    _projectiles.destroyAll();
    _projectiles.clear();
}

void GEProjectileManager::load(GEMapData*) {
    reset();
}

void GEProjectileManager::reset() {
    _projectiles.destroyAll();
    _projectiles.fillNull(Projectile::MAX_PROJECTILES);
}

void GEProjectileManager::addProjectile(ProjectileOwner from,
    float startPointX, float startPointY,
    float dirX, float dirY, float speed, int damage)
{
    const std::string texturePath =
        (from == ProjectileOwner::FromPlayer)
        ? Projectile::PLAYER_PROJECTILE_TEXTURE
        : Projectile::ENEMY_PROJECTILE_TEXTURE;

    // reuse inactive projectile slot if available
    for (unsigned int i = 0; i < _projectiles.size(); ++i) {
        GEProjectile* p = _projectiles[i];
        if (p == nullptr) {
            _projectiles[i] = new GEProjectile(texturePath, from,
                startPointX, startPointY, dirX, dirY, speed, damage);
            return;
        }
        if (!p->isActiveElement()) {
            p->spawn(texturePath, from,
                startPointX, startPointY, dirX, dirY, speed, damage);
            return;
        }
    }
}

void GEProjectileManager::update(float deltaTime, GEContext& ctx) {
    GEPlayer& player = static_cast<GEPlayer&>(ctx.playerProvider());
    const int enemyCount = ctx.enemyProvider().getEnemyCount();

    _projectiles.forEachActive([&](GEProjectile* projectile, unsigned int) {
        if (!projectile->isActiveElement()) return;

        GEProjectileMovementSystem::update(
            projectile->transformComponent(),
            projectile->projectileComponent(),
            deltaTime);

        if (projectile->getOwner() == ProjectileOwner::FromPlayer) {
            // player bullets hit enemies
            for (int j = 0; j < enemyCount; ++j) {
                GEEnemy* enemy = ctx.enemyProvider().getEnemyAt(j);
                if (enemy && enemy->isAlive() && projectile->collide(*enemy)) {
                    enemy->takeDamage(projectile->getDamage());
                    if (!enemy->isAlive()) {
                        ctx.enemyProvider().registerEnemyKill(enemy->getType());
                        ctx.powerupProvider().onEnemyDefeated(
                            GEPoint(enemy->getCenterX(), enemy->getCenterY()));
                    }
                    projectile->deactivate();
                    break;
                }
            }
        }
        else {
            // enemy bullets hit player
            if (projectile->collide(player.collisionBody())) {
                player.takeDamage(projectile->getDamage());
                projectile->deactivate();
            }
        }
        });

}

void GEProjectileManager::draw(Window& window, const GECamera& camera) {
    _projectiles.forEachActive([&](GEProjectile* projectile, unsigned int) {
        if (projectile && projectile->isActiveElement())
            projectile->draw(window, camera);
        });
}

GEProjectileManagerState GEProjectileManager::snapshotState() const {
    GEProjectileManagerState state;
    _projectiles.forEachActive([&](GEProjectile* projectile, unsigned int) {
        if (!projectile || !projectile->isActiveElement()) return;
        state.addProjectileState(projectile->snapshotState());
        });
    return state;
}

void GEProjectileManager::applyState(const GEProjectileManagerState& state) {
    _projectiles.destroyAll();
    _projectiles.fillNull(Projectile::MAX_PROJECTILES);

    state.forEachProjectile([&](const GEProjectileState& projectileState) {
        if (!projectileState.isActiveElement()) return;
        GEProjectile* projectile = new GEProjectile();
        projectile->applyState(projectileState);
        _projectiles.add(projectile);
        });
}
