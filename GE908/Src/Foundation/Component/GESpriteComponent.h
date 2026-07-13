#pragma once

#include "../../../ThirdParty/GamesEngineeringBase.h"
#include <string>

using namespace GamesEngineeringBase;

struct GESpriteAnimationClip {
    int firstFrame = 0;
    int frameCount = 1;
    float framesPerSecond = 0.0f;
    bool loop = true;

    GESpriteAnimationClip() = default;

    GESpriteAnimationClip(int firstFrame, int frameCount, float framesPerSecond, bool loop)
        : firstFrame(firstFrame),
        frameCount(frameCount),
        framesPerSecond(framesPerSecond),
        loop(loop) {
    }
};

class GESpriteComponent {
private:
    Image _image;
    int _frameWidth = 0;
    int _frameHeight = 0;
    int _columnCount = 0;
    int _totalFrameCount = 0;

    GESpriteAnimationClip _animation;
    int _currentAnimationFrame = 0;
    float _frameTimer = 0.0f;
    bool _playing = false;
    bool _finished = false;

    void resetAnimation() {
        _animation = GESpriteAnimationClip();
        _currentAnimationFrame = 0;
        _frameTimer = 0.0f;
        _playing = false;
        _finished = false;
    }

    void configureSingleFrame() {
        _frameWidth = static_cast<int>(_image.width);
        _frameHeight = static_cast<int>(_image.height);
        _columnCount = (_frameWidth > 0) ? 1 : 0;
        _totalFrameCount = (_frameWidth > 0 && _frameHeight > 0) ? 1 : 0;
        resetAnimation();
    }

    bool isSameAnimation(const GESpriteAnimationClip& animation) const {
        return _animation.firstFrame == animation.firstFrame
            && _animation.frameCount == animation.frameCount
            && _animation.framesPerSecond == animation.framesPerSecond
            && _animation.loop == animation.loop;
    }

public:
    bool load(const std::string& filename) {
        const bool loaded = _image.load(filename);
        if (loaded) configureSingleFrame();
        return loaded;
    }

    bool loadSpriteSheet(const std::string& filename, int frameWidth, int frameHeight) {
        if (!_image.load(filename)) return false;
        if (frameWidth <= 0 || frameHeight <= 0
            || frameWidth > static_cast<int>(_image.width)
            || frameHeight > static_cast<int>(_image.height)) {
            configureSingleFrame();
            return false;
        }

        _frameWidth = frameWidth;
        _frameHeight = frameHeight;
        // Frames are indexed left-to-right, then top-to-bottom.
        _columnCount = static_cast<int>(_image.width) / frameWidth;
        const int rowCount = static_cast<int>(_image.height) / frameHeight;
        _totalFrameCount = _columnCount * rowCount;
        resetAnimation();
        return _totalFrameCount > 0;
    }

    bool play(const GESpriteAnimationClip& animation, bool restart = false) {
        if (animation.firstFrame < 0
            || animation.frameCount <= 0
            || animation.firstFrame + animation.frameCount > _totalFrameCount) {
            return false;
        }
        if (!restart && isSameAnimation(animation)) {
            if (!_finished) {
                _playing = animation.frameCount > 1 && animation.framesPerSecond > 0.0f;
            }
            return true;
        }

        _animation = animation;
        _currentAnimationFrame = 0;
        _frameTimer = 0.0f;
        _playing = animation.frameCount > 1 && animation.framesPerSecond > 0.0f;
        _finished = false;
        return true;
    }

    bool showFrame(int frameIndex) {
        if (frameIndex < 0 || frameIndex >= _totalFrameCount) return false;
        return play(GESpriteAnimationClip(frameIndex, 1, 0.0f, false), true);
    }

    void stop(bool resetToFirstFrame = false) {
        _playing = false;
        _frameTimer = 0.0f;
        if (resetToFirstFrame) {
            _currentAnimationFrame = 0;
            _finished = false;
        }
    }

    void update(float deltaTime) {
        if (!_playing || deltaTime <= 0.0f) return;

        const float frameDuration = 1.0f / _animation.framesPerSecond;
        _frameTimer += deltaTime;

        while (_playing && _frameTimer >= frameDuration) {
            _frameTimer -= frameDuration;
            ++_currentAnimationFrame;

            if (_currentAnimationFrame < _animation.frameCount) continue;
            if (_animation.loop) {
                _currentAnimationFrame = 0;
            }
            else {
                _currentAnimationFrame = _animation.frameCount - 1;
                _playing = false;
                _finished = true;
            }
        }
    }

    int getCurrentFrameIndex() const {
        return _animation.firstFrame + _currentAnimationFrame;
    }

    int getFrameSourceX() const {
        return (_columnCount > 0) ? (getCurrentFrameIndex() % _columnCount) * _frameWidth : 0;
    }

    int getFrameSourceY() const {
        return (_columnCount > 0) ? (getCurrentFrameIndex() / _columnCount) * _frameHeight : 0;
    }

    int getWidth() const { return _frameWidth; }
    int getHeight() const { return _frameHeight; }
    int getTotalFrameCount() const { return _totalFrameCount; }
    bool isPlaying() const { return _playing; }
    bool isFinished() const { return _finished; }

    unsigned char* atUnchecked(int x, int y) const {
        return _image.atUnchecked(getFrameSourceX() + x, getFrameSourceY() + y);
    }

    unsigned char alphaAtUnchecked(int x, int y) const {
        return _image.alphaAtUnchecked(getFrameSourceX() + x, getFrameSourceY() + y);
    }
};
