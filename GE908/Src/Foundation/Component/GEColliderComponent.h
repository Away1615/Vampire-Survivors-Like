#pragma once

#include "../GEModel.h"

class GEColliderComponent {
private:
    GECollisionLayer _layer = GECollisionLayer::None;
    GEColliderShape _shape = GEColliderShape::None;
    float _offsetX = 0.0f;
    float _offsetY = 0.0f;
    float _radius = 0.0f;
    float _halfWidth = 0.0f;
    float _halfHeight = 0.0f;

public:
    explicit GEColliderComponent(GECollisionLayer layer = GECollisionLayer::None)
        : _layer(layer) {
    }

    GECollisionLayer getLayer() const { return _layer; }
    GEColliderShape getShape() const { return _shape; }
    float getOffsetX() const { return _offsetX; }
    float getOffsetY() const { return _offsetY; }
    float getRadius() const { return _radius; }
    float getHalfWidth() const { return _halfWidth; }
    float getHalfHeight() const { return _halfHeight; }
    float getBoundingHalfWidth() const {
        return _shape == GEColliderShape::Circle ? _radius : _halfWidth;
    }
    float getBoundingHalfHeight() const {
        return _shape == GEColliderShape::Circle ? _radius : _halfHeight;
    }

    void setCircle(float radius, float offsetX = 0.0f, float offsetY = 0.0f) {
        _shape = GEColliderShape::Circle;
        _radius = radius;
        _halfWidth = radius;
        _halfHeight = radius;
        _offsetX = offsetX;
        _offsetY = offsetY;
    }

    void setAabb(float width, float height, float offsetX = 0.0f, float offsetY = 0.0f) {
        _shape = GEColliderShape::Aabb;
        _radius = 0.0f;
        _halfWidth = width / 2.0f;
        _halfHeight = height / 2.0f;
        _offsetX = offsetX;
        _offsetY = offsetY;
    }
};
