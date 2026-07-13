#pragma once
#include "../../ThirdParty/GamesEngineeringBase.h"

using namespace GamesEngineeringBase;

class GEDebug {

private:

    bool _lastFramePressedCollision = false;
    bool _lastFramePressedAOE = false;
    bool _drawCollisionBounds = false;

    GEDebug() = default;

    void updateCollisionToggle(Window& window) {
        bool pressed = window.keyPressed('J');
        if (pressed && !_lastFramePressedCollision) 
            _drawCollisionBounds = !_drawCollisionBounds;
        _lastFramePressedCollision = pressed;
    }


public:
    static GEDebug& shared() {
        static GEDebug state;
        return state;
    }

    void updateFromInput(Window& window) {
        updateCollisionToggle(window);
    }

    bool needDrawCollisionBounds() const { return _drawCollisionBounds; }
};