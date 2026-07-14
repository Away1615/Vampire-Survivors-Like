#include "GameManager.h"
#include "Core/Session/GEGameSession.h"
#include "Foundation/GEFrameTimer.h"
#include <string>

using namespace GamesEngineeringBase;

static constexpr int KEY_ENTER = 13;
static constexpr int KEY_ESCAPE = 27;
static constexpr int KEY_UP = 38;
static constexpr int KEY_DOWN = 40;
static constexpr int KEY_DELETE = 46;
static constexpr int MAX_VISIBLE_SAVES = 8;

void GameManager::run() {
    _isRunning = true;

    while (_isRunning) {
        GEFrameTimer::shared().beginFrame();
        float deltaTime = GEFrameTimer::shared().getDeltaTime();
        updateInput();

        switch (_gameState) {
        case GEGameLifeCircle::Menu:
            updateMenu();
            renderMenu();
            break;
        case GEGameLifeCircle::SaveList:
            updateSaveList();
            renderSaveList();
            break;
        case GEGameLifeCircle::Playing:
            if (!_session.isActive()) {
                if (!_session.start(_mapMode)) {
                    stop();
                    return;
                }
            }
            _session.update(deltaTime);
            if (_session.isDefeated()) {
                _gameState = GEGameLifeCircle::Defeat;
            }
            else if (_session.isVictory()) {
                _gameState = GEGameLifeCircle::Victory;
            }
            else if (isKeyJustPressed('L')) {
                openSaveList(GESaveListMode::Save, GEGameLifeCircle::Playing);
            }
            _session.render();
            break;
        default:
            updateEnding();
            renderEnding();
            break;
        }
        GEFrameTimer::shared().endFrame();
    }
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
    _session.reset();
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

void GameManager::saveSelected() {
    GESaveRecord record;
    record.snapshot = _session.snapshot();
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
        || !_session.restore(record.snapshot)) {
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
                text = entry.displayName;
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
    _font.draw("Up/Down Select  Enter Confirm",
        GEPoint(80, 420), BLUE, _window);
    _font.draw("Delete Remove  Esc Back",
        GEPoint(80, 446), BLUE, _window);
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
