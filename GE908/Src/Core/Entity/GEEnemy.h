#pragma once
#include "GECharacter.h"
#include "../Interface/GEProvider.h"
#include "../../Foundation/GEObjectPool.h"
#include "../State/GECodable.h"
#include "../Component/GEEnemyComponent.h"

class GEPlayer;

class GEEnemy : public GECharacter, public GEPoolable, public GECodable<GEEnemyState> {
private:
	GEEnemyComponent _enemy;

	void bind(GEContext& ctx) override;
	void resetForSpawn(GEEnemyType type);

	void applyMovementBounds(float& newX, float& newY) override;
public:
	GEEnemy(GEEnemyType type);
	~GEEnemy();

	GEEnemyType getType() const { return _enemy.getType(); }
	void spawn(GEEnemyType type, float centerX, float centerY, int mapWidth, int mapHeight, bool infiniteMap);
	void update(float deltaTime, float playerCenterX, float playerCenterY, ProjectileProvider& projectileProvider);
	bool getIsStatic() const { return _enemy.isStationary(); }
	void draw(Window& window, const GECamera& camera) const override;
	void takeDamage(int value) override;

	float getAttackCooldown() const { return _enemy.getAttackCooldown(); }
	void setAttackCooldown(float value) { _enemy.setAttackCooldown(value); }

	GEEnemyComponent& enemyComponent() { return _enemy; }
	const GEEnemyComponent& enemyComponent() const { return _enemy; }

	bool isActiveElement() const override { return isAlive(); }

	GEEnemyState snapshotState() const override;
	void applyState(const GEEnemyState& state) override;
};
