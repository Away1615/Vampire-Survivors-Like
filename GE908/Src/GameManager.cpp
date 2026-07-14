#include "GameManager.h"
#include "Foundation/GEDebug.h"
#include <string>

#define WINDOW_WIDTH 854
#define WINDOW_HEIGHT 480

using namespace GamesEngineeringBase;

static constexpr const char* MAP_FILE_PATH = "Src/Assets/Maps/tilt.txt";
static constexpr int KEY_ENTER = 13;
static constexpr int KEY_ESCAPE = 27;
static constexpr int KEY_UP = 38;
static constexpr int KEY_DOWN = 40;
static constexpr int KEY_DELETE = 46;
static constexpr int MAX_VISIBLE_SAVES = 8;

void GameManager::run() {
    _isRunning = true;
    _font.load();
    _mapProvider.loadTileResources("Src/Assets/MapTiles/");

    while (_isRunning) {
        GEFrameTimer::shared().beginFrame();
        float deltaTime = GEFrameTimer::shared().getDeltaTime();
        updateInput();

        switch (_gameState) {
        case GEGameLifeCircle::Menu:
            _levelTimeRemaining = 120.0f;
            updateMenu();
            renderMenu();
            break;
        case GEGameLifeCircle::SaveList:
            updateSaveList();
            renderSaveList();
            break;
        case GEGameLifeCircle::Playing:
            if (!_componentHasLoaded) {
                loadComponent(_mapMode);
                _componentHasLoaded = true;
            }
            update(deltaTime);
            render();
            _levelTimeRemaining -= deltaTime;
            break;
        default:
            updateEnding();
            renderEnding();
            break;
        }
        GEFrameTimer::shared().endFrame();
    }
}

void GameManager::loadComponent(GEMapMode mapMode) {
    _mapData.load(MAP_FILE_PATH, mapMode);
    _levelTimeRemaining = 120.0f;

    _mapData.setWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    _mapProvider.load(&_mapData);

    _player.reset();
    _player.bind(_ctx);

    int mapWorldWidth = _mapData.isInfiniteMap() ? -1 : (_mapData.getActiveChunkPixelWidth() > 0 ? _mapData.getActiveChunkPixelWidth() : WINDOW_WIDTH);
    int mapWorldHeight = _mapData.isInfiniteMap() ? -1 : (_mapData.getActiveChunkPixelHeight() > 0 ? _mapData.getActiveChunkPixelHeight() : WINDOW_HEIGHT);
    _camera.load(WINDOW_WIDTH, WINDOW_HEIGHT, mapWorldWidth, mapWorldHeight);

    _enemyProvider.load(&_mapData);
    _powerUpProvider.load(&_mapData);
    _projectileProvider.load(&_mapData);
}

void GameManager::update(float deltaTime) {
    GEDebug::shared().updateFromInput(_window);

    _player.update(deltaTime, _window);

    // Get the position of the player's collider in each frame, and call followPlayer to update the camera
    auto& body = _player.collisionBody();
    _camera.followPlayer(body.getOriginX(), body.getOriginY(), body.getWidth(), body.getHeight());

    _mapData.setCameraOffset(_camera.getX(), _camera.getY());
    _mapData.updateActiveChunkFromWorldPosition(body.getCenterX(), body.getCenterY());

    _enemyProvider.update(deltaTime, _ctx);
    _projectileProvider.update(deltaTime, _ctx);
    _powerUpProvider.update(deltaTime, _ctx);

    if (_player.getHP() <= 0) {
        _gameState = GEGameLifeCircle::Defeat;
    }

    if (_player.getHP() > 0 && _levelTimeRemaining <= 0) {
        _gameState = GEGameLifeCircle::Victory;
    }

    if (_gameState == GEGameLifeCircle::Playing && isKeyJustPressed('L')) {
        openSaveList(GESaveListMode::Save, GEGameLifeCircle::Playing);
    }
}

void GameManager::render() {
    _window.clear();
    _mapProvider.draw(_window, _camera);
    _enemyProvider.draw(_window, _camera);
    _projectileProvider.draw(_window, _camera);
    _player.draw(_window, _camera);
    _powerUpProvider.draw(_window, _camera);

    drawText();
    _window.present();
}

void GameManager::drawText() {
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
        GEPoint(20, y=440), RED, _window);

    _font.draw("Time: " + std::to_string(static_cast<int>(std::ceil(_levelTimeRemaining))),
        GEPoint(540, 20), RED, _window);
    _font.draw("J  Show Collidars.", GEPoint(540, 400), BLUE, _window);
    _font.draw("L  Save Game.", GEPoint(540, 440), BLUE, _window);
}

void GameManager::stop() {
    _isRunning = false;
}

void GameManager::updateInput() {
    _window.checkInput();
    for (int key = 0; key < KEY_COUNT; ++key) {
        const bool isPressed = _window.keyPressed(key);
        _keyPressedThisFrame[key] = isPressed && !_previousKeyState[key];
        _previousKeyState[key] = isPressed;
    }
}

bool GameManager::isKeyJustPressed(int key) const {
    return key >= 0 && key < KEY_COUNT && _keyPressedThisFrame[key];
}

void GameManager::resetSession() {
    _enemyProvider.reset();
    _projectileProvider.reset();
    _powerUpProvider.reset();
    _player.reset();
    _mapProvider.reset();
    _levelTimeRemaining = 120.0f;
    _componentHasLoaded = false;
    _saveEntries.clear();
    _saveListStatus.clear();
}

void GameManager::openSaveList(GESaveListMode mode, GEGameLifeCircle returnState) {
    _saveListMode = mode;
    _saveListReturnState = returnState;
    _selectedSaveIndex = 0;
    _saveListStatus.clear();
    refreshSaveList();
    _gameState = GEGameLifeCircle::SaveList;
}

void GameManager::refreshSaveList() {
    _saveEntries = _saveRepository.list();
    const int itemCount = getSaveListItemCount();
    if (itemCount <= 0) {
        _selectedSaveIndex = 0;
    }
    else if (_selectedSaveIndex >= itemCount) {
        _selectedSaveIndex = itemCount - 1;
    }
}

int GameManager::getSaveListItemCount() const {
    const int createItemCount = _saveListMode == GESaveListMode::Save ? 1 : 0;
    return static_cast<int>(_saveEntries.size()) + createItemCount;
}

int GameManager::getSelectedEntryIndex() const {
    const int createItemCount = _saveListMode == GESaveListMode::Save ? 1 : 0;
    return _selectedSaveIndex - createItemCount;
}

GESaveRecord GameManager::captureSaveRecord() const {
    GESaveRecord record;
    record.snapshot.map.mapMode = _mapMode;
    record.snapshot.map.hasRandomSeed = _mapData.hasRandomSeed();
    record.snapshot.map.randomSeed = _mapData.getRandomSeed();
    record.snapshot.map.activeChunkX = _mapData.getActiveChunkX();
    record.snapshot.map.activeChunkY = _mapData.getActiveChunkY();
    record.snapshot.map.cameraX = _camera.getX();
    record.snapshot.map.cameraY = _camera.getY();
    record.snapshot.map.levelTimeRemaining = _levelTimeRemaining;
    record.snapshot.player = _player.snapshotState();
    record.snapshot.enemies = _enemyProvider.snapshotState();
    record.snapshot.projectiles = _projectileProvider.snapshotState();
    record.snapshot.powerUps = _powerUpProvider.snapshotState();
    return record;
}

bool GameManager::restoreSaveRecord(const GESaveRecord& record) {
    const GEGameSnapshot& snapshot = record.snapshot;
    if (!_mapData.load(MAP_FILE_PATH, snapshot.map.mapMode)) return false;

    _mapMode = snapshot.map.mapMode;
    _mapData.setWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    _mapData.restoreRuntimeState(
        snapshot.map.hasRandomSeed,
        snapshot.map.randomSeed,
        snapshot.map.activeChunkX,
        snapshot.map.activeChunkY);
    _mapProvider.load(&_mapData);

    _player.reset();
    _player.bind(_ctx);
    _player.applyState(snapshot.player);
    _mapData.restoreRuntimeState(
        snapshot.map.hasRandomSeed,
        snapshot.map.randomSeed,
        snapshot.map.activeChunkX,
        snapshot.map.activeChunkY);

    const int mapWorldWidth = _mapData.isInfiniteMap()
        ? -1
        : (_mapData.getActiveChunkPixelWidth() > 0 ? _mapData.getActiveChunkPixelWidth() : WINDOW_WIDTH);
    const int mapWorldHeight = _mapData.isInfiniteMap()
        ? -1
        : (_mapData.getActiveChunkPixelHeight() > 0 ? _mapData.getActiveChunkPixelHeight() : WINDOW_HEIGHT);
    _camera.load(WINDOW_WIDTH, WINDOW_HEIGHT, mapWorldWidth, mapWorldHeight);
    _camera.setPosition(snapshot.map.cameraX, snapshot.map.cameraY);
    _mapData.setCameraOffset(snapshot.map.cameraX, snapshot.map.cameraY);

    _enemyProvider.load(&_mapData);
    _enemyProvider.applyState(snapshot.enemies);
    _projectileProvider.load(&_mapData);
    _projectileProvider.applyState(snapshot.projectiles);
    _powerUpProvider.load(&_mapData);
    _powerUpProvider.applyState(snapshot.powerUps);

    _levelTimeRemaining = snapshot.map.levelTimeRemaining;
    _componentHasLoaded = true;
    return true;
}

void GameManager::saveSelected() {
    GESaveRecord record = captureSaveRecord();
    const int entryIndex = getSelectedEntryIndex();
    if (entryIndex >= 0 && entryIndex < static_cast<int>(_saveEntries.size())) {
        record.metadata = _saveEntries[entryIndex];
    }

    if (_saveRepository.save(record)) {
        _gameState = _saveListReturnState;
        return;
    }
    _saveListStatus = "Unable to save game.";
}

void GameManager::loadSelected() {
    const int entryIndex = getSelectedEntryIndex();
    if (entryIndex < 0 || entryIndex >= static_cast<int>(_saveEntries.size())) return;

    GESaveRecord record;
    if (!_saveRepository.load(_saveEntries[entryIndex].id, record)
        || !restoreSaveRecord(record)) {
        _saveListStatus = "Unable to load save.";
        return;
    }
    _gameState = GEGameLifeCircle::Playing;
}

void GameManager::deleteSelected() {
    const int entryIndex = getSelectedEntryIndex();
    if (entryIndex < 0 || entryIndex >= static_cast<int>(_saveEntries.size())) return;

    if (!_saveRepository.remove(_saveEntries[entryIndex].id)) {
        _saveListStatus = "Unable to delete save.";
        return;
    }
    _saveListStatus = "Save deleted.";
    refreshSaveList();
}

void GameManager::updateSaveList() {
    if (isKeyJustPressed(KEY_ESCAPE)) {
        _gameState = _saveListReturnState;
        return;
    }

    const int itemCount = getSaveListItemCount();
    if (itemCount <= 0) return;

    if (isKeyJustPressed(KEY_UP)) {
        _selectedSaveIndex = (_selectedSaveIndex + itemCount - 1) % itemCount;
    }
    else if (isKeyJustPressed(KEY_DOWN)) {
        _selectedSaveIndex = (_selectedSaveIndex + 1) % itemCount;
    }
    else if (isKeyJustPressed(KEY_DELETE)) {
        deleteSelected();
    }
    else if (isKeyJustPressed(KEY_ENTER)) {
        if (_saveListMode == GESaveListMode::Save) saveSelected();
        else loadSelected();
    }
}

void GameManager::renderSaveList() {
    _window.clear();
    const std::string title = _saveListMode == GESaveListMode::Save
        ? "Save Game"
        : "Load Game";
    _font.draw(title, GEPoint(80, 40), WHITE, _window);

    const int itemCount = getSaveListItemCount();
    if (itemCount == 0) {
        _font.draw("No saves found.", GEPoint(80, 100), WHITE, _window);
    }
    else {
        int firstVisible = 0;
        if (_selectedSaveIndex >= MAX_VISIBLE_SAVES) {
            firstVisible = _selectedSaveIndex - MAX_VISIBLE_SAVES + 1;
        }
        const int lastVisible = firstVisible + MAX_VISIBLE_SAVES < itemCount
            ? firstVisible + MAX_VISIBLE_SAVES
            : itemCount;

        for (int itemIndex = firstVisible; itemIndex < lastVisible; ++itemIndex) {
            std::string text;
            if (_saveListMode == GESaveListMode::Save && itemIndex == 0) {
                text = "[Create New Save]";
            }
            else {
                const int entryIndex = itemIndex - (_saveListMode == GESaveListMode::Save ? 1 : 0);
                const GESaveMetadata& entry = _saveEntries[entryIndex];
                const std::string mapName = entry.mapMode == GEMapMode::Infinite ? "Infinite" : "Fixed";
                text = entry.displayName
                    + "  " + mapName
                    + "  HP:" + std::to_string(entry.playerHp)
                    + "  Time:" + std::to_string(static_cast<int>(entry.levelTimeRemaining));
            }

            const std::string prefix = itemIndex == _selectedSaveIndex ? "> " : "  ";
            const GEColor color = itemIndex == _selectedSaveIndex ? GREEN : WHITE;
            const int y = 90 + (itemIndex - firstVisible) * 36;
            _font.draw(prefix + text, GEPoint(80, y), color, _window);
        }
    }

    if (!_saveListStatus.empty()) {
        _font.draw(_saveListStatus, GEPoint(80, 390), RED, _window);
    }
    _font.draw("Up/Down Select  Enter Confirm  Delete Remove  Esc Back",
        GEPoint(80, 430), BLUE, _window);
    _window.present();
}

void GameManager::updateMenu() {
    if (isKeyJustPressed('1')) {
        _gameState = GEGameLifeCircle::Playing;
        _mapMode = GEMapMode::Fixed;
    }
    else if (isKeyJustPressed('2')) {
        _gameState = GEGameLifeCircle::Playing;
        _mapMode = GEMapMode::Infinite;
    }
    else if (isKeyJustPressed('3')) {
        openSaveList(GESaveListMode::Load, GEGameLifeCircle::Menu);
    }
}


void GameManager::renderMenu() {
    _window.clear();
    _font.draw("Press 1 for new Fixed Map", GEPoint(160, 160), WHITE, _window);
    _font.draw("Press 2 for new Infinite Map", GEPoint(160, 200), WHITE, _window);
    _font.draw("Press 3 to load a saved game", GEPoint(160, 240), WHITE, _window);
    _window.present();
}


void GameManager::updateEnding() {
    if (isKeyJustPressed(' ')) {
        resetSession();
        _gameState = GEGameLifeCircle::Menu;
    }
}

void GameManager::renderEnding() {
    _window.clear();
    if (_gameState == GEGameLifeCircle::Defeat) {
        _font.draw("Failed", GEPoint(200, 200), RED, _window);
        _font.draw("Press Space Back To Menu.", GEPoint(200, 240), WHITE, _window);
    }

    if (_gameState == GEGameLifeCircle::Victory) {
        _font.draw("Win!", GEPoint(200, 200), GREEN, _window);
        _font.draw("Press Space Back To Menu.", GEPoint(200, 240), WHITE, _window);
    }
    
    _window.present();
}
