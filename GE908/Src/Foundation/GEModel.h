#pragma once

enum class GEPowerUpType {
	None					= 0,
	AttackSpeedBoost		= 1,
	AdditionalAoeTarget		= 2,
	HealPlayer				= 3,
};

enum class ProjectileOwner {
	FromPlayer				= 0,
	FromEnemy				= 1,
};

enum class GEEnemyType {
	Normal					= 0,
	Fast					= 1,
	Heavy					= 2,
	StaticShooter			= 3,
};

enum class GECollisionLayer {
	None,
	Player,
	Enemy,
	TerrainSolid,
	TerrainHazard,
	Projectile,
	PowerUp,
};

enum class GEColliderShape {
	None,
	Circle,
	Aabb,
};

enum class GEGameLifeCircle {
	Menu,
	SaveList,
	Playing,
	Defeat,
	Victory,
};

enum class GEMapMode {
	Fixed,
	Infinite,
};
