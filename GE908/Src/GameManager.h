#pragma once
#include "Foundation/GEFont.h"
#include "Foundation/GEFrameTimer.h"
#include "Foundation/GECamera.h"
#include "Core/Map/GEMapData.h"
#include "Core/Save/GESaveRepository.h"
#include "Core/Entity/GEPlayer.h"
#include "Core/Interface/GEProvider.h"
#include "Foundation/GEContext.h"
#include <array>
#include <vector>

using namespace GamesEngineeringBase;

class GameManager {
private:
    Window& _window;
    GEFont _font;
    GECamera _camera;
    GEMapData _mapData;
    GESaveRepository _saveRepository;
    MapProvider& _mapProvider;
    PlayerProvider& _player;
    EnemyProvider& _enemyProvider;
    ProjectileProvider& _projectileProvider;
    PowerUpProvider& _powerUpProvider;
    GEContext _ctx;
    bool _isRunning = false;

    GEGameLifeCircle _gameState = GEGameLifeCircle::Menu;
    GEMapMode _mapMode = GEMapMode::Fixed;
    GESaveListMode _saveListMode = GESaveListMode::Load;
    GEGameLifeCircle _saveListReturnState = GEGameLifeCircle::Menu;
    std::vector<GESaveMetadata> _saveEntries;
    int _selectedSaveIndex = 0;
    std::string _saveListStatus;

    float _levelTimeRemaining = 120.0f;

    bool _componentHasLoaded = false;
    static constexpr int KEY_COUNT = 256;
    std::array<bool, KEY_COUNT> _previousKeyState{};
    std::array<bool, KEY_COUNT> _keyPressedThisFrame{};

    void updateInput();
    bool isKeyJustPressed(int key) const;
    void resetSession();
    void openSaveList(GESaveListMode mode, GEGameLifeCircle returnState);
    void refreshSaveList();
    int getSaveListItemCount() const;
    int getSelectedEntryIndex() const;
    GESaveRecord captureSaveRecord() const;
    bool restoreSaveRecord(const GESaveRecord& record);
    void saveSelected();
    void loadSelected();
    void deleteSelected();
    void updateSaveList();
    void renderSaveList();

public:
    GameManager(Window& window,
        MapProvider& mapProvider,
        PlayerProvider& playerProvider,
        EnemyProvider& enemyProvider,
        ProjectileProvider& projectileProvider,
        PowerUpProvider& powerUpProvider)
        : _window(window),
        _mapProvider(mapProvider),
        _player(playerProvider),
        _enemyProvider(enemyProvider),
        _projectileProvider(projectileProvider),
        _powerUpProvider(powerUpProvider),
        _ctx(window, enemyProvider, mapProvider, projectileProvider, powerUpProvider, playerProvider)
    {
        _gameState = GEGameLifeCircle::Menu;
    }

    ~GameManager() = default;

	// Start Game Loops
	void run();

	// load Game Resource, like , player, enemies, fonts
	void loadComponent(GEMapMode mapMode);

	// update
	void update(float deltaTime);

	// render
	void render();

	// End Game Loops
	void stop();

    void updateMenu();

    void renderMenu();

    void updateEnding();

    void renderEnding();

	void drawText();

	GameManager(const GameManager&) = delete;
	GameManager& operator=(const GameManager&) = delete;
};

