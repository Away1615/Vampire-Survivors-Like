#pragma once
#include <string>
#include "../Map/GEMapData.h"
#include "../../Foundation/GECollisible.h"
#include "../../Foundation/GEModel.h"
#include "../State/GECodable.h"
#include "../State/GEGameState.h"

struct GEContext;

class GETile;
class GEEnemy;

class EnemyProvider : public GECodable<GEEnemyManagerState> {
public:
	virtual ~EnemyProvider() = default;
	virtual void load(GEMapData* mapData) = 0;
	virtual void reset() = 0;
	virtual void update(float deltaTime, GEContext& ctx) = 0;
	virtual int getEnemyCount() const = 0;
	virtual GEEnemy* getEnemyAt(int index) const = 0;
	virtual void registerEnemyKill(GEEnemyType type) = 0;
	virtual int getKillCount(GEEnemyType type) const = 0;
	virtual void draw(Window& window, const GECamera& camera) = 0;
};

class ProjectileProvider : public GECodable<GEProjectileManagerState> {
public:
	virtual ~ProjectileProvider() = default;
	virtual void load(GEMapData* mapData) = 0;
	virtual void reset() = 0;
	virtual void update(float deltaTime, GEContext& ctx) = 0;
	virtual void addProjectile(ProjectileOwner from, float startPointX, float startPointY, float dirX, float dirY, float speed, int damage) = 0;
	virtual void draw(Window& window, const GECamera& camera) = 0;
};

class PowerUpProvider : public GECodable<GEPowerUpManagerState> {
public:
	virtual ~PowerUpProvider() = default;
	virtual void load(GEMapData* mapData) = 0;
	virtual void reset() = 0;
	virtual void update(float deltaTime, GEContext& ctx) = 0;
	virtual void draw(Window& window, const GECamera& camera) = 0;
	virtual void onEnemyDefeated(const GEPoint& position) = 0;
};

class MapProvider {
public:
	virtual ~MapProvider() = default;
	virtual GEMapData* getMapData() const = 0;
	virtual GETile* getTile(int tileID) const = 0;
	virtual GECollisionLayer getTileCollisionLayer(int tileID) const = 0;
	virtual void draw(Window& window, const GECamera& camera) const = 0;
	virtual void loadTileResources(const std::string& folderPath) = 0;
	virtual void load(GEMapData* mapData) = 0;
	virtual void reset() = 0;
};


class PlayerProvider : public GECodable<GEPlayerState> {
public:
	virtual ~PlayerProvider() = default;
	virtual void reset() = 0;
	virtual void takeDamage(int value) = 0;
	virtual void update(float deltaTime, Window& window) = 0;
	virtual GECollisible& collisionBody() = 0;
	virtual bool canReceiveContactDamage() const = 0;
	virtual void startContactDamageCooldown() = 0;
	virtual void applyPowerUp(GEPowerUpType type) = 0;
	virtual int getHP() const = 0;
	virtual float getAOECooldownTime() const = 0;
	virtual void draw(Window& window, const GECamera& camera) const = 0;
	virtual void bind(GEContext& ctx) = 0;

};

