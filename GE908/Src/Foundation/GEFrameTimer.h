#pragma once
#include "../../ThirdParty/GamesEngineeringBase.h"

using namespace GamesEngineeringBase;

class GEFrameTimer : public Timer {
private:
    // Frame timing state
    float _deltaTime = 0.0f;
    float _fps = 0.0f;
    float _accumTime = 0.0f;
    int _frameCount = 0;
    const float _updateInterval = 1.0f;// seconds between FPS updates

    GEFrameTimer() = default;
    ~GEFrameTimer() = default;

    GEFrameTimer(const GEFrameTimer&) = delete;
    GEFrameTimer& operator=(const GEFrameTimer&) = delete;

public:

    static GEFrameTimer& shared() {
        static GEFrameTimer instance;
        return instance;
    }

    // start measuring this frame
    void beginFrame() {
        reset();
    }

    // finish measuring this frame and compute FPS
    void endFrame() {
        _deltaTime = dt();
        _frameCount++;
        _accumTime += _deltaTime;

        // update FPS once per second
        if (_accumTime >= _updateInterval) {
            _fps = static_cast<float>(_frameCount) / _accumTime;
            _accumTime = 0.0f;
            _frameCount = 0;
        }
    }

    float getDeltaTime() const { return _deltaTime;}
    float getFPS() const { return _fps;}
};
