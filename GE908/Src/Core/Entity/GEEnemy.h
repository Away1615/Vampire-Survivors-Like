#pragma once
#include "GECharacter.h"
#include "../Interface/GEProvider.h"
#include "../../Foundation/GEObjectPool.h"
#include "../State/GECodable.h"

class GEPlayer;

class GEEnemy : public GECharacter, public GEPoolable, public GECodable<GEEnemyState> {
private:
	GEEnemyType _type = GEEnemyType::Normal;
	float _attackCooldown = 0.0f;
	bool _stationary = false;

	void configure(GEEnemyType type);
	void resetForSpawn(GEEnemyType type, const Image& texture);
	void updateRangedAttack(float deltaTime,
		float playerCenterX,
		float playerCenterY,
		ProjectileProvider& projectileProvider);

	void applyMovementBounds(float& newX, float& newY) override;
public:
	GEEnemy(GEEnemyType type, const Image& texture);
	~GEEnemy();

	GEEnemyType getType() const { return _type; }
	void spawn(GEEnemyType type, const Image& texture,
		float centerX, float centerY, int mapWidth, int mapHeight, bool infiniteMap);
	void update(float deltaTime, float playerCenterX, float playerCenterY, ProjectileProvider& projectileProvider);
	bool getIsStatic() const { return _stationary; }
	void draw(Window& window, const GECamera& camera) const override;
	void takeDamage(int value) override;

	float getAttackCooldown() const { return _attackCooldown; }
	void setAttackCooldown(float value) { _attackCooldown = value; }

	bool isActiveElement() const override { return isAlive(); }

	GEEnemyState snapshotState() const override;
	void applyState(const GEEnemyState& state) override;
};
