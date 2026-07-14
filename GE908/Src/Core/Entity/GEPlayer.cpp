#include "GEPlayer.h"
#include "../Map/GEMapData.h"
#include "../../Foundation/GEDebug.h"
#include "GEEnemy.h"
#include "../System/GEPlayerCombatSystem.h"
#include <cmath>

GEPlayer::GEPlayer(const Image& frontWalkTexture,
    const Image& backWalkTexture,
    const Image& sideWalkTexture)
    : GECharacter(frontWalkTexture, GECollisionLayer::Player),
    _frontWalkTexture(frontWalkTexture),
    _backWalkTexture(backWalkTexture),
    _sideWalkTexture(sideWalkTexture) {
    setCircleCollider(Player::COLLISION_RADIUS);
    reset();
}

void GEPlayer::reset() {
    _walkDirection = WalkDirection::Front;
    setWalkSprite(_frontWalkTexture);
    setSpriteFlipHorizontal(false);
    setSpriteDrawOffsetX(Player::FRONT_BACK_WALK_DRAW_OFFSET_X);
    setSpriteDrawOffsetY(Player::WALK_DRAW_OFFSET_Y);
    stopSpriteAnimation(true);
    _mapData = nullptr;
    _mapsManager = nullptr;
    _enemyManager = nullptr;
    _projectileManager = nullptr;
    _powerUpManager = nullptr;

    _hp = 500;
    _speed = Player::MOVE_SPEED;
    _maxHp = _hp;
    _mapWidth = 0;
    _mapHeight = 0;
    _player.reset();
    _damageFlashTimer = 0.0f;
    _damageFlashDuration = 0.0f;
    _contactDamageCooldownTimer = 0.0f;
    setContactDamageCooldownDuration(0.5f);
    setCenter(0.0f, 0.0f);
}

void GEPlayer::setWalkSprite(const Image& texture) {
    const int frameWidth = static_cast<int>(texture.width) / Player::WALK_FRAME_COUNT;
    setSpriteSheet(texture, frameWidth, static_cast<int>(texture.height));
}

void GEPlayer::setWalkDirection(WalkDirection direction) {
    if (_walkDirection == direction) return;
    _walkDirection = direction;

    switch (direction) {
    case WalkDirection::Front:
        setWalkSprite(_frontWalkTexture);
        break;
    case WalkDirection::Back:
        setWalkSprite(_backWalkTexture);
        break;
    case WalkDirection::Side:
        setWalkSprite(_sideWalkTexture);
        break;
    }
}

void GEPlayer::updateWalkAnimation(float dirX, float dirY) {
    if (dirX == 0.0f && dirY == 0.0f) {
        stopSpriteAnimation(true);
        return;
    }

    if (dirX != 0.0f) {
        setWalkDirection(WalkDirection::Side);
        const bool facingLeft = dirX < 0.0f;
        setSpriteFlipHorizontal(facingLeft);
        setSpriteDrawOffsetX(facingLeft
            ? -Player::SIDE_WALK_DRAW_OFFSET_X
            : Player::SIDE_WALK_DRAW_OFFSET_X);
    }
    else if (dirY < 0.0f) {
        setWalkDirection(WalkDirection::Back);
        setSpriteFlipHorizontal(false);
        setSpriteDrawOffsetX(Player::FRONT_BACK_WALK_DRAW_OFFSET_X);
    }
    else {
        setWalkDirection(WalkDirection::Front);
        setSpriteFlipHorizontal(false);
        setSpriteDrawOffsetX(Player::FRONT_BACK_WALK_DRAW_OFFSET_X);
    }

    playSpriteAnimation(GESpriteAnimationClip(
        0,
        Player::WALK_FRAME_COUNT,
        Player::WALK_FRAMES_PER_SECOND,
        true));
}

void GEPlayer::bind(MapProvider& mapProvider,
    EnemyProvider& enemyProvider,
    ProjectileProvider& projectileProvider,
    PowerUpProvider& powerUpProvider) {
    _mapsManager = &mapProvider;
    _enemyManager = &enemyProvider;
    _projectileManager = &projectileProvider;
    _powerUpManager = &powerUpProvider;

    if (_mapsManager) {
        _mapData = _mapsManager->getMapData();
        if (_mapData) {
            int chunkPixelW = _mapData->getChunkPixelWidth();
            int chunkPixelH = _mapData->getChunkPixelHeight();
            if (chunkPixelW <= 0) chunkPixelW = getWidth() * 4;
            if (chunkPixelH <= 0) chunkPixelH = getHeight() * 4;

            if (_mapData->isInfiniteMap())
                setMapBounds(-1, -1);
            else
                setMapBounds(chunkPixelW, chunkPixelH);

            setCenter(chunkPixelW / 2.0f, chunkPixelH / 2.0f);
            float boundedX = getCenterX();
            float boundedY = getCenterY();
            applyMovementBounds(boundedX, boundedY);
            setCenter(boundedX, boundedY);

            _mapData->updateActiveChunkFromWorldPosition(getCenterX(), getCenterY());
        }
    }
}


void GEPlayer::update(float deltaTime, Window& window) {
    updateCharacterState(deltaTime);
    _player.updateBuffs(deltaTime);

    float dirX = 0.0f;
    float dirY = 0.0f;

    if (window.keyPressed('W')) dirY -= 1.0f;
    if (window.keyPressed('S')) dirY += 1.0f;
    if (window.keyPressed('A')) dirX -= 1.0f;
    if (window.keyPressed('D')) dirX += 1.0f;

    if (window.keyPressed('Q')) _player.requestAoe();

    updateWalkAnimation(dirX, dirY);
    updateSpriteAnimation(deltaTime);
    moveUpdate(deltaTime, dirX, dirY);

    GEPlayerCombatSystem::update(
        _player,
        transformComponent(),
        deltaTime,
        _enemyManager,
        _projectileManager,
        _powerUpManager);

    applyEnvironmentalEffects(deltaTime);
}

bool GEPlayer::collidesWithTileLayer(float newX, float newY, const MapProvider& maps, GECollisionLayer targetLayer) const {
    if (!_mapData) return false;

    const int tileW = _mapData->getTileWidth();
    const int tileH = _mapData->getTileHeight();
    if (tileW <= 0 || tileH <= 0) return false;

    const GEColliderComponent& playerCollider = colliderComponent();
    const float colliderCenterX = newX + playerCollider.getOffsetX();
    const float colliderCenterY = newY + playerCollider.getOffsetY();
    const int minTileX = static_cast<int>(std::floor(
        (colliderCenterX - playerCollider.getBoundingHalfWidth()) / tileW));
    const int maxTileX = static_cast<int>(std::floor(
        (colliderCenterX + playerCollider.getBoundingHalfWidth()) / tileW));
    const int minTileY = static_cast<int>(std::floor(
        (colliderCenterY - playerCollider.getBoundingHalfHeight()) / tileH));
    const int maxTileY = static_cast<int>(std::floor(
        (colliderCenterY + playerCollider.getBoundingHalfHeight()) / tileH));

    GEColliderComponent tileCollider(targetLayer);
    tileCollider.setAabb(static_cast<float>(tileW), static_cast<float>(tileH));

    for (int tileY = minTileY; tileY <= maxTileY; ++tileY) {
        for (int tileX = minTileX; tileX <= maxTileX; ++tileX) {

            int tileID = _mapData->getTileID(tileY, tileX);
            if (maps.getTileCollisionLayer(tileID) != targetLayer) continue;

            const float tileCenterX = tileX * tileW + tileW / 2.0f;
            const float tileCenterY = tileY * tileH + tileH / 2.0f;
            if (GECollisionQuery::overlaps(
                newX,
                newY,
                playerCollider,
                tileCenterX,
                tileCenterY,
                tileCollider)) return true;
        }
    }

    return false;
}

bool GEPlayer::collidesWithEnemies(float newX, float newY, const EnemyProvider& enemyManager) const {
    int enemyCount = enemyManager.getEnemyCount();
    for (int i = 0;i < enemyCount;i++) {
        GEEnemy* enemy = enemyManager.getEnemyAt(i);

        if (!enemy || !enemy->isAlive()) continue;

        const bool currentlyColliding = this->collide(*enemy);
        if (!this->collideAt(newX, newY, *enemy))
            continue;

        if (!currentlyColliding)
            return true;

        const float currentDx = getCenterX() - enemy->getCenterX();
        const float currentDy = getCenterY() - enemy->getCenterY();
        const float newDx = newX - enemy->getCenterX();
        const float newDy = newY - enemy->getCenterY();

        const float currentDistSq = currentDx * currentDx + currentDy * currentDy;
        const float newDistSq = newDx * newDx + newDy * newDy;

        if (newDistSq <= currentDistSq)
            return true;
    }
    return false;
}

bool GEPlayer::isBlockedAt(float x, float y) const {
    if (!_mapData) return false;

    if (collidesWithTileLayer(x, y, *_mapsManager, GECollisionLayer::TerrainSolid))
        return true;

    if (collidesWithEnemies(x, y, *_enemyManager))
        return true;

    return false;
}

void GEPlayer::applyEnvironmentalEffects(float deltaTime) {
    bool inFire = collidesWithTileLayer(
        getCenterX(),
        getCenterY(),
        *_mapsManager,
        GECollisionLayer::TerrainHazard);

    if (inFire) {
        if (!_player.wasInFire()) {
            takeDamage(Player::FIRE_DAMAGE);
            _player.setFireTimer(0.0f);
        }
        else {
            const float fireTimer = _player.getFireTimer() + deltaTime;
            _player.setFireTimer(fireTimer);
            if (fireTimer >= Player::FIRE_DAMAGE_INTERVAL) {
                takeDamage(Player::FIRE_DAMAGE);
                _player.setFireTimer(0.0f);
            }
        }
    }
    else {
        _player.setFireTimer(0.0f);
    }

    _player.setWasInFire(inFire);
}

void GEPlayer::applyMovementBounds(float& newX, float& newY) {
    if (!_mapData || _mapData->isInfiniteMap() || _mapWidth <= 0 || _mapHeight <= 0) return;

    float camW = static_cast<float>(_mapData->getScreenWidth());
    float camH = static_cast<float>(_mapData->getScreenHeight());

    float mapW = static_cast<float>(_mapWidth);
    float mapH = static_cast<float>(_mapHeight);

    float minCenterX = camW / 2.0f;
    float maxCenterX = mapW - camW / 2.0f;
    float minCenterY = camH / 2.0f;
    float maxCenterY = mapH - camH / 2.0f;

    newX = clamp(newX, minCenterX, maxCenterX);
    newY = clamp(newY, minCenterY, maxCenterY);
}

void GEPlayer::drawAoeImpacts(Window& window, const GECamera& camera) const {
    const float impactRadius = 30.0f;
    for (int i = 0; i < _player.getAoeImpactCount(); ++i) {
        const GEPlayerAoeImpact& impact = _player.getAoeImpact(i);
        if (impact.remainingTime > 0.0f) {
            drawImpact(window, camera, impact.x, impact.y, impactRadius, GREEN);
        }
    }
}

void GEPlayer::drawImpact(Window& window, const GECamera& camera, float centerX, float centerY, float radius, GEColor color) const {
    const float camX = camera.getX();
    const float camY = camera.getY();

    int winW = window.getWidth();
    int winH = window.getHeight();

    int radiusInt = static_cast<int>(radius);
    if (radiusInt <= 0) {
        return;
    }
    int cx = static_cast<int>(centerX - camX);
    int cy = static_cast<int>(centerY - camY);
    int radiusSq = radiusInt * radiusInt;

    for (int dx = -radiusInt;dx <= radiusInt;++dx) {
        int rem = radiusSq - dx * dx;
        if (rem < 0) continue;
        int dy = static_cast<int>(std::sqrt(static_cast<float>(rem)));

        int x = cx + dx;
        if (x < 0 || x >= winW) continue;

        int y1 = cy + dy;
        int y2 = cy - dy;
        if (y1 >= 0 && y1 < winH) window.draw(x, y1, color.r, color.g, color.b);
        if (y2 >= 0 && y2 < winH) window.draw(x, y2, color.r, color.g, color.b);
    }
}

void GEPlayer::draw(Window& window, const GECamera& camera) const {
    GECharacter::draw(window, camera);
    drawAoeImpacts(window, camera);
}

void GEPlayer::applyPowerUp(GEPowerUpType type) {
    switch (type) {
    case GEPowerUpType::AttackSpeedBoost:
        _player.applyAttackSpeedBoost();
        break;
    case GEPowerUpType::AdditionalAoeTarget:
        _player.applyAdditionalAoeTarget();
        break;
    case GEPowerUpType::HealPlayer:
        heal(Player::PLAYER_HEAL_VALUE);
        break;
    }
}

void GEPlayer::takeDamage(int value) {
    if (value <= 0) return;
    GECharacter::takeDamage(value);
    triggerDamageFlash(GEColor(255, 0, 0), 0.25f);
}

GEPlayerState GEPlayer::snapshotState() const {
    GEPlayerState state;
    state.centerX = getCenterX();
    state.centerY = getCenterY();
    state.hp = _hp;
    state.maxHp = _maxHp;
    state.speed = _speed;
    state.autoAttackTimer = _player.getAutoAttackTimer();
    state.autoAttackSpeedMultiplier = _player.getAutoAttackSpeedMultiplier();
    state.attackSpeedBuffTimer = _player.getAttackSpeedBuffTimer();
    state.aoeCooldownTimer = _player.getAoeCooldownTimer();
    state.aoeCooldown = _player.getAoeCooldown();
    state.contactDamageCooldownTimer = _contactDamageCooldownTimer;
    state.wasInFire = _player.wasInFire();
    state.fireTimer = _player.getFireTimer();
    state.aoeTargetCount = _player.getAoeTargetCount();
    state.aoeKeyHeld = _player.isAoeRequested();
    state.aoeTargetBuffTimer = _player.getAoeTargetBuffTimer();
    return state;
}

void GEPlayer::applyState(const GEPlayerState& state) {
    _speed = state.speed > 0 ? state.speed : _speed;
    if (state.maxHp > 0) setMaxHP(state.maxHp);
    setCurrentHP(state.hp);
    _contactDamageCooldownTimer = max(0.0f, state.contactDamageCooldownTimer);
    _player.restore(
        state.autoAttackTimer,
        state.autoAttackSpeedMultiplier,
        state.attackSpeedBuffTimer,
        state.aoeCooldownTimer,
        state.aoeCooldown,
        state.aoeTargetCount,
        state.aoeKeyHeld,
        state.aoeTargetBuffTimer);
    _player.setWasInFire(state.wasInFire);
    _player.setFireTimer(max(0.0f, state.fireTimer));

    float newX = state.centerX;
    float newY = state.centerY;
    applyMovementBounds(newX, newY);
    setCenter(newX, newY);
}
