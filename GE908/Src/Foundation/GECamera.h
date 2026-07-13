#pragma once
#include "GEUtility.h"

class GECamera {
private:
	float _x = 0;
	float _y = 0;

	int _width = 0;
	int _height = 0;

	int _mapWidth = 0;
	int _mapHeight = 0;

public:

	GECamera() = default;
	~GECamera() = default;

	float getX() const { return _x; }
	float getY() const { return _y; }

	int getWidth() const { return _width; }
	int getHeight() const { return _height; }

    void load(int windowWidth, int windowHeight, int mapWidth, int mapHeight) {
		_width = windowWidth;
		_height = windowHeight;
		_mapWidth = mapWidth;
		_mapHeight = mapHeight;
		_x = 0;
		_y = 0;
	}

    void followPlayer(float playerX, float playerY, int playerWidth, int playerHeight) {
        float targetX = playerX + (playerWidth / 2.0f) - (_width / 2.0f);
        float targetY = playerY + (playerHeight / 2.0f) - (_height / 2.0f);

        if (_mapWidth > _width)
            _x = clamp(targetX, 0, _mapWidth - _width);
        else if (_mapWidth > 0)
            _x = (_mapWidth - _width) / 2;
        else
            _x = targetX;

        if (_mapHeight > _height)
            _y = clamp(targetY, 0, _mapHeight - _height);
        else if (_mapHeight > 0)
            _y = (_mapHeight - _height) / 2;
        else
            _y = targetY;
    }

    void setMapBounds(int mapWidth, int mapHeight) {
        _mapWidth = mapWidth;
        _mapHeight = mapHeight;
        if (_mapWidth <= 0) _mapWidth = -1;
        if (_mapHeight <= 0) _mapHeight = -1;
    }

    void setPosition(float x, float y) {
        _x = x;
        _y = y;
    }
};