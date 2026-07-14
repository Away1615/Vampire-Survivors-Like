#include "GEPowerUpManager.h"
#include "../GEContext.h"
#include "../System/GEPowerUpLifetimeSystem.h"
#include <cstdlib>
#include <ctime>

GEPowerUpManager::GEPowerUpManager() {
    _powerUps.fillNull(PowerUp::MAX_POWERUPS);
}

GEPowerUpManager::~GEPowerUpManager() {
    _powerUps.destroyAll();
    _powerUps.clear();
}

void GEPowerUpManager::load(GEMapData* mapData) {
    reset();
    _mapData = mapData;
}

void GEPowerUpManager::reset() {
    _mapData = nullptr;
    _spawnTimer = 0.0f;
    _powerUps.destroyAll();
    _powerUps.fillNull(PowerUp::MAX_POWERUPS);
}

void GEPowerUpManager::spawnPowerUpAt(const GEPoint& point) {
    // Random drop chance
    if (randomFloat(0.0f, 1.0f) > PowerUp::DROP_CHANCE) return;

    float offsetX = randomFloat(-20.0f, 20.0f);
    float offsetY = randomFloat(-20.0f, 20.0f);
    float spawnX = point.x + offsetX;
    float spawnY = point.y + offsetY;

    float randomN = randomFloat(0.0f, 1.0f);

    GEPowerUpType type = GEPowerUpType::AttackSpeedBoost;
    if (randomN < 0.33) type = GEPowerUpType::AttackSpeedBoost;
    if (randomN >= 0.33 && randomN <= 0.66) type = GEPowerUpType::AdditionalAoeTarget;
    if (randomN > 0.66) type = GEPowerUpType::HealPlayer;

    // Try to reuse inactive slot
    for (unsigned int i = 0; i < _powerUps.size(); ++i) {
        if (_powerUps[i] == nullptr) {
            _powerUps[i] = new GEPowerUp();
        }
        if (!_powerUps[i]->isAlive()) {
            _powerUps[i]->spawn(type, spawnX, spawnY, PowerUp::POWERUP_LIFETIME_SECONDS);
            return;
        }
    }
}

void GEPowerUpManager::update(float deltaTime, GEContext& ctx) {
    if (!_mapData) return;

    _spawnTimer += deltaTime;
    if (_spawnTimer >= PowerUp::SPAWN_INTERVAL_SECONDS) {
        _spawnTimer = 0.0f;
        // could spawn periodic global pickups here if desired
    }

    PlayerProvider& player = ctx.playerProvider();

    for (unsigned int i = 0; i < _powerUps.size(); ++i) {
        GEPowerUp* p = _powerUps[i];
        if (!p || !p->isAlive()) continue;
        GEPowerUpLifetimeSystem::update(p->powerUpComponent(), deltaTime);

        if (p->isAlive() && p->collide(player.collisionBody())) {
            player.applyPowerUp(p->getType());
            p->deactivate();
        }
    }

}

void GEPowerUpManager::draw(Window& window, const GECamera& camera) {
    for (unsigned int i = 0; i < _powerUps.size(); ++i) {
        GEPowerUp* p = _powerUps[i];
        if (p && p->isAlive())
            p->draw(window, camera);
    }
}

void GEPowerUpManager::onEnemyDefeated(const GEPoint& position) {
    spawnPowerUpAt(position);
}

GEPowerUpManagerState GEPowerUpManager::snapshotState() const {
    GEPowerUpManagerState state;
    state.spawnTimer = _spawnTimer;
    for (unsigned int i = 0; i < _powerUps.size(); ++i) {
        GEPowerUp* powerUp = _powerUps[i];
        if (!powerUp || !powerUp->isAlive()) continue;
        GEPowerUpState powerUpState = powerUp->snapshotState();
        state.addPowerUpState(powerUpState);
    }
    return state;
}

void GEPowerUpManager::applyState(const GEPowerUpManagerState& state) {
    _spawnTimer = state.spawnTimer;
    _powerUps.destroyAll();
    _powerUps.fillNull(PowerUp::MAX_POWERUPS);

    for (unsigned int i = 0; i < state.powerUps.size(); ++i) {
        GEPowerUpState* powerUpState = state.powerUps[i];
        if (!powerUpState || !powerUpState->isActiveElement()) continue;
        GEPowerUp* powerUp = new GEPowerUp();
        powerUp->applyState(*powerUpState);
        _powerUps.add(powerUp);
    }
}
