#pragma once
#include "../../ThirdParty/GamesEngineeringBase.h"
#include "../Foundation/GECamera.h"
#include "../Foundation/GEModel.h"
#include "Component/GETransformComponent.h"
#include "Component/GESpriteComponent.h"
#include "Component/GEColliderComponent.h"
#include "GECollisionQuery.h"
#include "GEDebug.h"
#include <cmath>

using namespace GamesEngineeringBase;
	
class GECollisible {

private:
	GETransformComponent _transform;
	GESpriteComponent _sprite;
	GEColliderComponent _collider;

protected:
	bool loadSprite(const std::string& filename) {
		return _sprite.load(filename);
	}

	bool loadSpriteSheet(const std::string& filename, int frameWidth, int frameHeight) {
		return _sprite.loadSpriteSheet(filename, frameWidth, frameHeight);
	}

	bool playSpriteAnimation(const GESpriteAnimationClip& animation, bool restart = false) {
		return _sprite.play(animation, restart);
	}

	void updateSpriteAnimation(float deltaTime) { _sprite.update(deltaTime); }
	void stopSpriteAnimation(bool resetToFirstFrame = false) { _sprite.stop(resetToFirstFrame); }
	bool isSpriteAnimationFinished() const { return _sprite.isFinished(); }

	void setCircleCollider(float radius, float offsetX = 0.0f, float offsetY = 0.0f) {
		_collider.setCircle(radius, offsetX, offsetY);
	}

	void setAabbCollider(float width, float height, float offsetX = 0.0f, float offsetY = 0.0f) {
		_collider.setAabb(width, height, offsetX, offsetY);
	}

public:

	GECollisible(const std::string& filename, GECollisionLayer collisionLayer)
		: _collider(collisionLayer) {
		loadSprite(filename);
	}

	virtual ~GECollisible() = default;

	float getOriginX() const { return _transform.getCenterX() - _sprite.getWidth() / 2.0f;}
	float getOriginY() const { return _transform.getCenterY() - _sprite.getHeight() / 2.0f;}
	float getCenterX() const { return _transform.getCenterX();}
	float getCenterY() const { return _transform.getCenterY();}
	int getWidth() const { return _sprite.getWidth();}
	int getHeight() const { return _sprite.getHeight();}
	float getCollisionRadius() const { return _collider.getRadius(); }
	GECollisionLayer getCollisionLayer() const { return _collider.getLayer(); }

    void setCenter(float centerX, float centerY) { _transform.setCenter(centerX, centerY); }

    GETransformComponent& transformComponent() { return _transform; }
    const GETransformComponent& transformComponent() const { return _transform; }
    const GESpriteComponent& spriteComponent() const { return _sprite; }
    const GEColliderComponent& colliderComponent() const { return _collider; }

    // draw collider's image
	virtual void draw(Window& window, const GECamera& camera) const {
		const float camX = camera.getX();
		const float camY = camera.getY();
		int winW = window.getWidth();
		int winH = window.getHeight();

		for (int dy = 0; dy < _sprite.getHeight(); ++dy) {
			const int screenY = static_cast<int>(getOriginY() + dy - camY);
			if (screenY < 0 || screenY >= winH) continue;

			for (int dx = 0; dx < _sprite.getWidth(); ++dx) {
				const int screenX = static_cast<int>(getOriginX() + dx - camX);
				if (screenX < 0 || screenX >= winW) continue;

				if (_sprite.alphaAtUnchecked(dx, dy) > 0)
					window.draw(screenX, screenY, _sprite.atUnchecked(dx, dy));
			}
		}

		drawCollisionBoxIfNeeded(window, camera);
	}

    // draw collider's collider box
    virtual void drawCollisionBoxIfNeeded(Window& window, const GECamera& camera) const {

		if (!GEDebug::shared().needDrawCollisionBounds()
			|| _collider.getLayer() == GECollisionLayer::None
			|| _collider.getShape() == GEColliderShape::None) return;

        const float camX = camera.getX();
        const float camY = camera.getY();
        int winW = window.getWidth();
        int winH = window.getHeight();

		const float colliderCenterX = getCenterX() + _collider.getOffsetX();
		const float colliderCenterY = getCenterY() + _collider.getOffsetY();

		if (_collider.getShape() == GEColliderShape::Aabb) {
			int left = static_cast<int>(colliderCenterX - _collider.getHalfWidth() - camX);
			int top = static_cast<int>(colliderCenterY - _collider.getHalfHeight() - camY);
			int right = static_cast<int>(colliderCenterX + _collider.getHalfWidth() - camX);
			int bottom = static_cast<int>(colliderCenterY + _collider.getHalfHeight() - camY);

            if (right <= 0 || bottom <= 0 || left >= winW || top >= winH) {

            }
            else {
                if (left < 0)   left = 0;
                if (top < 0)    top = 0;
                if (right > winW) right = winW;
                if (bottom > winH) bottom = winH;

                const int width = right - left;
                const int height = bottom - top;
                if (width > 0 && height > 0) {
                    const int xL = left;
                    const int xR = right - 1;
                    const int yT = top;
                    const int yB = bottom - 1;

                    for (int x = xL; x <= xR; ++x) {
                        if (yT >= 0 && yT < winH)
                            window.draw(x, yT, 0, 0, 255);
                        if (yB >= 0 && yB < winH)
                            window.draw(x, yB, 0, 0, 255);
                    }

                    for (int y = yT; y <= yB; ++y) {
                        if (xL >= 0 && xL < winW)
                            window.draw(xL, y, 0, 0, 255);
                        if (xR >= 0 && xR < winW)
                            window.draw(xR, y, 0, 0, 255);
                    }
                }
            }
        }
        else {
			const int radius = static_cast<int>(std::ceil(getCollisionRadius()));
			const int cx = static_cast<int>(colliderCenterX - camX);
			const int cy = static_cast<int>(colliderCenterY - camY);
            const int r2 = radius * radius;

            for (int dx = -radius; dx <= radius; ++dx) {
                const int rem = r2 - dx * dx;
                if (rem < 0) continue;
                const int dy = static_cast<int>(sqrtf(static_cast<float>(rem)));

                const int x = cx + dx;
                if (x < 0 || x >= winW) continue;

                const int y1 = cy + dy;
                const int y2 = cy - dy;
                if (y1 >= 0 && y1 < winH)
                    window.draw(x, y1, 255, 0, 0);
                if (y2 >= 0 && y2 < winH)
                    window.draw(x, y2, 255, 0, 0);
            }
        }

    }
   
	// check if this collider collide with another right now
	bool collide(const GECollisible& other) const {
		return GECollisionQuery::overlaps(
			_transform,
			_collider,
			other._transform,
			other._collider);
	}

	// check if this collider would collide with another when placed at position (cx, cy), without actually moving it
	bool collideAt(float cx, float cy, const GECollisible& other) const {
		return GECollisionQuery::overlaps(
			cx,
			cy,
			_collider,
			other.getCenterX(),
			other.getCenterY(),
			other._collider);
	}
};
