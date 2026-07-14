#include "GEEnemy.h"
#include <cmath>

static float EnemyCollisionRadius(GEEnemyType type) {
	switch (type) {
    case GEEnemyType::Normal:        return Enemy::NORMAL_COLLISION_RADIUS;
    case GEEnemyType::Fast:          return Enemy::FAST_COLLISION_RADIUS;
    case GEEnemyType::Heavy:         return Enemy::HEAVY_COLLISION_RADIUS;
    case GEEnemyType::StaticShooter: return Enemy::STATIC_COLLISION_RADIUS;
	default:                         return Enemy::NORMAL_COLLISION_RADIUS;
	}
}

void GEEnemy::configure(GEEnemyType type) {
	_type = type;
	_attackCooldown = 0.0f;
	_stationary = false;

	if (type == GEEnemyType::StaticShooter) {
		_stationary = true;
	}
}

void GEEnemy::applyMovementBounds(float& newX, float& newY) {
    if (_mapWidth <= 0 || _mapHeight <= 0) return;

    float minCenterX = getWidth() / 2.0f;
    float maxCenterX = _mapWidth - getWidth() / 2.0f;
    float minCenterY = getHeight() / 2.0f;
    float maxCenterY = _mapHeight - getHeight() / 2.0f;

    newX = clamp(newX, minCenterX, maxCenterX);
    newY = clamp(newY, minCenterY, maxCenterY);
}

GEEnemy::GEEnemy(GEEnemyType type, const Image& texture)
    : GECharacter(texture, GECollisionLayer::Enemy), _type(type) {
    resetForSpawn(type, texture);
}

void GEEnemy::resetForSpawn(GEEnemyType type, const Image& texture) {
    if (_type != type) {
        setSprite(texture);
    }
    configure(type);
    setCircleCollider(EnemyCollisionRadius(type));
    stopSpriteAnimation(true);
    setContactDamageCooldownDuration(0.5f);
	_damageFlashTimer = 0.0f;
	_contactDamageCooldownTimer = 0.0f;

	switch (_type) {
    case GEEnemyType::Normal:
        _hp = 200;
        _speed = 150;
        _maxHp = _hp;
        break;
    case GEEnemyType::Fast:
        _hp = 100;
        _speed = 230;
        _maxHp = _hp;
        break;
    case GEEnemyType::Heavy:
        _hp = 300;
        _speed = 80;
        _maxHp = _hp;
        break;
    case GEEnemyType::StaticShooter:
        _hp = 150;
        _speed = 0;
        _maxHp = _hp;
        break;
    }
}

void GEEnemy::spawn(GEEnemyType type, const Image& texture,
    float centerX, float centerY, int mapWidth, int mapHeight, bool infiniteMap) {
    resetForSpawn(type, texture);
    setMapBounds(infiniteMap ? -1 : mapWidth, infiniteMap ? -1 : mapHeight);
    setCenter(centerX, centerY);
}

GEEnemy::~GEEnemy() = default;

void GEEnemy::update(float deltaTime, float playerCenterX, float playerCenterY, ProjectileProvider& projectileProvider) {
    updateCharacterState(deltaTime);
    updateSpriteAnimation(deltaTime);

    if (!isAlive()) return;

    if (!_stationary) {
        const float dx = playerCenterX - getCenterX();
        const float dy = playerCenterY - getCenterY();

        if (dx != 0.0f || dy != 0.0f) {
            moveUpdate(deltaTime, dx, dy);
        }
        return;
    }
    updateRangedAttack(deltaTime, playerCenterX, playerCenterY, projectileProvider);
}

void GEEnemy::updateRangedAttack(float deltaTime,
    float playerCenterX,
    float playerCenterY,
    ProjectileProvider& projectileProvider) {
    if (!_stationary) return;

    _attackCooldown += deltaTime;
    if (_attackCooldown < Enemy::RANGED_ATTACK_INTERVAL) return;
    _attackCooldown = 0.0f;

    const float centerX = getCenterX();
    const float centerY = getCenterY();
    float dirX = playerCenterX - centerX;
    float dirY = playerCenterY - centerY;
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
        Projectile::MOVE_SPEED,
        100);
}

void GEEnemy::draw(Window& window, const GECamera& camera) const {
    GECharacter::draw(window, camera);
}

void GEEnemy::takeDamage(int value) {
    if (value <= 0) return;
    GECharacter::takeDamage(value);
    triggerDamageFlash(GEColor(255, 255, 255), 0.25f);
}

GEEnemyState GEEnemy::snapshotState() const {
    GEEnemyState state;
    state.type = _type;
    state.centerX = getCenterX();
    state.centerY = getCenterY();
    state.hp = _hp;
    state.maxHp = _maxHp;
    state.attackCooldown = _attackCooldown;
    state.contactDamageCooldownTimer = _contactDamageCooldownTimer;
    return state;
}

void GEEnemy::applyState(const GEEnemyState& state) {
    if (state.maxHp > 0) setMaxHP(state.maxHp);
    setCurrentHP(state.hp);
    _attackCooldown = state.attackCooldown;
    _contactDamageCooldownTimer = max(0.0f, state.contactDamageCooldownTimer);
    setCenter(state.centerX, state.centerY);
}
