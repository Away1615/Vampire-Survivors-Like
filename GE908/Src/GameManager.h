#pragma once
#include "Foundation/GEFont.h"
#include "Foundation/GEModel.h"
#include "Core/Save/GESaveRepository.h"
#include <array>
#include <string>
#include <vector>

using namespace GamesEngineeringBase;

class GEGameSession;

// Runs the frame loop and switches game states.
class GameManager {
private:
    Window& _window;
    const GEFont& _font;
    GEGameSession& _session;
    GESaveRepository _saveRepository;
    bool _isRunning = false;

    GEGameLifeCircle _gameState = GEGameLifeCircle::Menu;
    GEMapMode _mapMode = GEMapMode::Fixed;
    GESaveListMode _saveListMode = GESaveListMode::Load;
    GEGameLifeCircle _saveListReturnState = GEGameLifeCircle::Menu;
    std::vector<GESaveMetadata> _saveEntries;
    int _selectedSaveIndex = 0;
    std::string _saveListStatus;

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
    void saveSelected();
    void loadSelected();
    void deleteSelected();
    void updateSaveList();
    void renderSaveList();
    void updateMenu();
    void renderMenu();
    void updateEnding();
    void renderEnding();

public:
    GameManager(Window& window,
        const GEFont& font,
        GEGameSession& session)
        : _window(window),
        _font(font),
        _session(session)
    {
        _gameState = GEGameLifeCircle::Menu;
    }

    ~GameManager() = default;

	void run();

	void stop();

	GameManager(const GameManager&) = delete;
	GameManager& operator=(const GameManager&) = delete;
};

