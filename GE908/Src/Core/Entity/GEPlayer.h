#pragma once
#include "GECharacter.h"
#include "GETile.h"
#include "../../Foundation/GEConst.h"
#include "../Interface/GEProvider.h"
#include "../../Foundation/GECamera.h"
#include "../../Foundation/GEModel.h"
#include "../Map/GEMapData.h"
#include "../Component/GEPlayerComponent.h"

class GEEnemy;

class GEPlayer : public GECharacter, public PlayerProvider, public GECodable<GEPlayerState> {

private:
	GEMapData* _mapData = nullptr;
	MapProvider* _mapsManager = nullptr;
	EnemyProvider* _enemyManager = nullptr;
	ProjectileProvider* _projectileManager = nullptr;
	PowerUpProvider* _powerUpManager = nullptr;
	GEPlayerComponent _player;

	bool collidesWithTileLayer(float newX, float newY, const MapProvider& maps, GECollisionLayer targetLayer) const;

	bool collidesWithEnemies(float newX, float newY, const EnemyProvider& enemyManager) const;

	bool isBlockedAt(float x, float y) const;

	void applyEnvironmentalEffects(float deltaTime);

	void applyMovementBounds(float& newX, float& newY) override;
	void drawAoeImpacts(Window& window, const GECamera& camera) const;
	void drawImpact(Window& window, const GECamera& camera, float centerX, float centerY, float radius, GEColor color) const;

public:

	GEPlayer();

	~GEPlayer() = default;

	void reset() override;

	void bind(GEContext& ctx) override;

	void update(float deltaTime, Window& window);

	void draw(Window& window, const GECamera& camera) const override;

	float getAOECooldownTime() const { return _player.getAoeCooldownTimer(); }

	void applyPowerUp(GEPowerUpType type);

	void takeDamage(int value) override;

	GECollisible& collisionBody() { return *this; }

	int getHP() const override { return _hp; }

	GEPlayerComponent& playerComponent() { return _player; }
	const GEPlayerComponent& playerComponent() const { return _player; }

	GEPlayerState snapshotState() const override;
	void applyState(const GEPlayerState& state) override;

};
