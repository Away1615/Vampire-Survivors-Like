#include "GEGameSession.h"
#include "../../Foundation/GEDebug.h"
#include "../../Foundation/GEFrameTimer.h"
#include <cmath>
#include <string>

static constexpr int WINDOW_WIDTH = 854;
static constexpr int WINDOW_HEIGHT = 480;
static constexpr float LEVEL_DURATION_SECONDS = 120.0f;

GEGameSession::GEGameSession(Window& window,
    const GEFont& font,
    MapProvider& mapProvider,
    PlayerProvider& playerProvider,
    EnemyProvider& enemyProvider,
    ProjectileProvider& projectileProvider,
    PowerUpProvider& powerUpProvider)
    : _window(window),
    _font(font),
    _mapProvider(mapProvider),
    _player(playerProvider),
    _enemyProvider(enemyProvider),
    _projectileProvider(projectileProvider),
    _powerUpProvider(powerUpProvider) {
}

bool GEGameSession::prepareWorld(GEMapMode mapMode) {
    if (!_mapProvider.load(&_mapData, mapMode)) return false;

    _mapMode = mapMode;
    _mapData.setWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    _player.reset();
    _player.bind(_mapProvider, _enemyProvider, _projectileProvider, _powerUpProvider);

    const int mapWorldWidth = _mapData.isInfiniteMap()
        ? -1
        : (_mapData.getActiveChunkPixelWidth() > 0
            ? _mapData.getActiveChunkPixelWidth()
            : WINDOW_WIDTH);
    const int mapWorldHeight = _mapData.isInfiniteMap()
        ? -1
        : (_mapData.getActiveChunkPixelHeight() > 0
            ? _mapData.getActiveChunkPixelHeight()
            : WINDOW_HEIGHT);
    _camera.load(WINDOW_WIDTH, WINDOW_HEIGHT, mapWorldWidth, mapWorldHeight);

    _enemyProvider.load(&_mapData);
    _powerUpProvider.load(&_mapData);
    _projectileProvider.reset();
    return true;
}

bool GEGameSession::start(GEMapMode mapMode) {
    if (!prepareWorld(mapMode)) return false;

    _levelTimeRemaining = LEVEL_DURATION_SECONDS;
    _isActive = true;
    _isDefeated = false;
    _isVictory = false;
    return true;
}

void GEGameSession::reset() {
    _enemyProvider.reset();
    _projectileProvider.reset();
    _powerUpProvider.reset();
    _player.reset();
    _mapProvider.reset();

    _levelTimeRemaining = LEVEL_DURATION_SECONDS;
    _isActive = false;
    _isDefeated = false;
    _isVictory = false;
}

void GEGameSession::update(float deltaTime) {
    GEDebug::shared().updateFromInput(_window);

    _player.update(deltaTime, _window);

    GECollisible& body = _player.collisionBody();
    _camera.followPlayer(body.getOriginX(), body.getOriginY(), body.getWidth(), body.getHeight());

    _mapData.setCameraOffset(_camera.getX(), _camera.getY());
    _mapData.updateActiveChunkFromWorldPosition(body.getCenterX(), body.getCenterY());

    _enemyProvider.update(deltaTime, _player, _projectileProvider, _powerUpProvider);
    _projectileProvider.update(
        deltaTime,
        _player,
        _enemyProvider,
        _powerUpProvider,
        _camera);
    _powerUpProvider.update(deltaTime, _player);

    _isDefeated = _player.getHP() <= 0;
    _isVictory = !_isDefeated && _levelTimeRemaining <= 0.0f;
    _levelTimeRemaining -= deltaTime;
}

void GEGameSession::render() {
    _window.clear();
    _mapProvider.draw(_window, _camera);
    _enemyProvider.draw(_window, _camera);
    _projectileProvider.draw(_window, _camera);
    _player.draw(_window, _camera);
    _powerUpProvider.draw(_window, _camera);
    drawHud();
    _window.present();
}

void GEGameSession::drawHud() {
    int y = 20;
    _font.draw("Normal: " + std::to_string(_enemyProvider.getKillCount(GEEnemyType::Normal)),
        GEPoint(20, y), RED, _window);
    y += 20;
    _font.draw("Fast: " + std::to_string(_enemyProvider.getKillCount(GEEnemyType::Fast)),
        GEPoint(20, y), RED, _window);
    y += 20;
    _font.draw("Heavy: " + std::to_string(_enemyProvider.getKillCount(GEEnemyType::Heavy)),
        GEPoint(20, y), RED, _window);
    y += 20;
    _font.draw("Static: " + std::to_string(_enemyProvider.getKillCount(GEEnemyType::StaticShooter)),
        GEPoint(20, y), RED, _window);
    _font.draw("FPS: " + std::to_string(static_cast<int>(GEFrameTimer::shared().getFPS())),
        GEPoint(20, 400), RED, _window);
    _font.draw("HP: " + std::to_string(_player.getHP()),
        GEPoint(20, 420), RED, _window);
    _font.draw("Skill: " + std::to_string(static_cast<int>(_player.getAOECooldownTime())),
        GEPoint(20, 440), RED, _window);

    _font.draw("Time: " + std::to_string(static_cast<int>(std::ceil(_levelTimeRemaining))),
        GEPoint(540, 20), RED, _window);
    _font.draw("J  Show Collidars.", GEPoint(540, 400), BLUE, _window);
    _font.draw("L  Save Game.", GEPoint(540, 440), BLUE, _window);
}

GEGameSnapshot GEGameSession::snapshot() const {
    GEGameSnapshot state;
    state.map.mapMode = _mapMode;
    state.map.hasRandomSeed = _mapData.hasRandomSeed();
    state.map.randomSeed = _mapData.getRandomSeed();
    state.map.activeChunkX = _mapData.getActiveChunkX();
    state.map.activeChunkY = _mapData.getActiveChunkY();
    state.map.cameraX = _camera.getX();
    state.map.cameraY = _camera.getY();
    state.map.levelTimeRemaining = _levelTimeRemaining;
    state.player = _player.snapshotState();
    state.enemies = _enemyProvider.snapshotState();
    state.projectiles = _projectileProvider.snapshotState();
    state.powerUps = _powerUpProvider.snapshotState();
    return state;
}

bool GEGameSession::restore(const GEGameSnapshot& state) {
    if (!prepareWorld(state.map.mapMode)) return false;

    // Apply runtime state after loading static world data.
    _player.applyState(state.player);
    _mapData.restoreRuntimeState(
        state.map.hasRandomSeed,
        state.map.randomSeed,
        state.map.activeChunkX,
        state.map.activeChunkY);
    _camera.setPosition(state.map.cameraX, state.map.cameraY);
    _mapData.setCameraOffset(state.map.cameraX, state.map.cameraY);

    _enemyProvider.applyState(state.enemies);
    _projectileProvider.applyState(state.projectiles);
    _powerUpProvider.applyState(state.powerUps);

    _levelTimeRemaining = state.map.levelTimeRemaining;
    _isActive = true;
    _isDefeated = false;
    _isVictory = false;
    return true;
}
