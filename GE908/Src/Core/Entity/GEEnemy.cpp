#include "GEEnemy.h"
#include "../System/GEEnemyRangedAttackSystem.h"
#include <string>

static const std::string EnemyImagePath(GEEnemyType t) {
	switch (t) {
    case GEEnemyType::Normal:        return "Src/Assets/Textures/enemy_normal.png";
    case GEEnemyType::Fast:          return "Src/Assets/Textures/enemy_fast.png";
    case GEEnemyType::Heavy:         return "Src/Assets/Textures/enemy_heavy.png";
    case GEEnemyType::StaticShooter: return "Src/Assets/Textures/enemy_static.png";
	default:                         return "Src/Assets/Textures/enemy_normal.png";
	}
}

static float EnemyCollisionRadius(GEEnemyType type) {
	switch (type) {
    case GEEnemyType::Normal:        return Enemy::NORMAL_COLLISION_RADIUS;
    case GEEnemyType::Fast:          return Enemy::FAST_COLLISION_RADIUS;
    case GEEnemyType::Heavy:         return Enemy::HEAVY_COLLISION_RADIUS;
    case GEEnemyType::StaticShooter: return Enemy::STATIC_COLLISION_RADIUS;
	default:                         return Enemy::NORMAL_COLLISION_RADIUS;
	}
}

void GEEnemy::bind(GEContext& ctx) {
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

GEEnemy::GEEnemy(GEEnemyType type)
    : GECharacter(EnemyImagePath(type), GECollisionLayer::Enemy), _enemy(type) {
    resetForSpawn(type);
}

void GEEnemy::resetForSpawn(GEEnemyType type) {
    if (_enemy.getType() != type) {
        loadSprite(EnemyImagePath(type));
    }
    _enemy.configure(type);
    setCircleCollider(EnemyCollisionRadius(type));
    stopSpriteAnimation(true);
    setContactDamageCooldownDuration(0.5f);
	_damageFlashTimer = 0.0f;
	_contactDamageCooldownTimer = 0.0f;

	switch (_enemy.getType()) {
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

void GEEnemy::spawn(GEEnemyType type, float centerX, float centerY, int mapWidth, int mapHeight, bool infiniteMap) {
    resetForSpawn(type);
    setMapBounds(infiniteMap ? -1 : mapWidth, infiniteMap ? -1 : mapHeight);
    setCenter(centerX, centerY);
}

GEEnemy::~GEEnemy() = default;

void GEEnemy::update(float deltaTime, float playerCenterX, float playerCenterY, ProjectileProvider& projectileProvider) {
    updateCharacterState(deltaTime);
    updateSpriteAnimation(deltaTime);

    if (!isAlive()) return;

    if (!_enemy.isStationary()) {
        const float dx = playerCenterX - getCenterX();
        const float dy = playerCenterY - getCenterY();

        if (dx != 0.0f || dy != 0.0f) {
            moveUpdate(deltaTime, dx, dy);
        }
        return;
    }
    GEEnemyRangedAttackSystem::update(
        _enemy,
        transformComponent(),
        playerCenterX,
        playerCenterY,
        deltaTime,
        projectileProvider);
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
    state.type = _enemy.getType();
    state.centerX = getCenterX();
    state.centerY = getCenterY();
    state.hp = _hp;
    state.maxHp = _maxHp;
    state.attackCooldown = _enemy.getAttackCooldown();
    state.activate();
    return state;
}

void GEEnemy::applyState(const GEEnemyState& state) {
    if (state.maxHp > 0) setMaxHP(state.maxHp);
    setCurrentHP(state.hp);
    _enemy.setAttackCooldown(state.attackCooldown);
    setCenter(state.centerX, state.centerY);
}
