#pragma once

class GETransformComponent {
private:
    float _centerX = 0.0f;
    float _centerY = 0.0f;

public:
    float getCenterX() const { return _centerX; }
    float getCenterY() const { return _centerY; }

    void setCenter(float centerX, float centerY) {
        _centerX = centerX;
        _centerY = centerY;
    }

    void translate(float deltaX, float deltaY) {
        _centerX += deltaX;
        _centerY += deltaY;
    }
};
