#pragma once

#include "Component/GEColliderComponent.h"
#include "Component/GETransformComponent.h"

class GECollisionQuery {
private:
    static float clampValue(float value, float minimum, float maximum) {
        if (value < minimum) return minimum;
        if (value > maximum) return maximum;
        return value;
    }

    static bool circleCircle(
        float firstX,
        float firstY,
        float firstRadius,
        float secondX,
        float secondY,
        float secondRadius) {
        const float dx = firstX - secondX;
        const float dy = firstY - secondY;
        const float combinedRadius = firstRadius + secondRadius;
        return dx * dx + dy * dy <= combinedRadius * combinedRadius;
    }

    static bool aabbAabb(
        float firstX,
        float firstY,
        float firstHalfWidth,
        float firstHalfHeight,
        float secondX,
        float secondY,
        float secondHalfWidth,
        float secondHalfHeight) {
        return firstX - firstHalfWidth <= secondX + secondHalfWidth
            && firstX + firstHalfWidth >= secondX - secondHalfWidth
            && firstY - firstHalfHeight <= secondY + secondHalfHeight
            && firstY + firstHalfHeight >= secondY - secondHalfHeight;
    }

    static bool circleAabb(
        float circleX,
        float circleY,
        float radius,
        float boxX,
        float boxY,
        float boxHalfWidth,
        float boxHalfHeight) {
        const float closestX = clampValue(
            circleX,
            boxX - boxHalfWidth,
            boxX + boxHalfWidth);
        const float closestY = clampValue(
            circleY,
            boxY - boxHalfHeight,
            boxY + boxHalfHeight);
        const float dx = circleX - closestX;
        const float dy = circleY - closestY;
        return dx * dx + dy * dy <= radius * radius;
    }

public:
    static bool overlaps(
        float firstCenterX,
        float firstCenterY,
        const GEColliderComponent& first,
        float secondCenterX,
        float secondCenterY,
        const GEColliderComponent& second) {
        if (first.getLayer() == GECollisionLayer::None
            || second.getLayer() == GECollisionLayer::None
            || first.getShape() == GEColliderShape::None
            || second.getShape() == GEColliderShape::None) {
            return false;
        }

        const float firstX = firstCenterX + first.getOffsetX();
        const float firstY = firstCenterY + first.getOffsetY();
        const float secondX = secondCenterX + second.getOffsetX();
        const float secondY = secondCenterY + second.getOffsetY();

        if (first.getShape() == GEColliderShape::Circle
            && second.getShape() == GEColliderShape::Circle) {
            return circleCircle(
                firstX,
                firstY,
                first.getRadius(),
                secondX,
                secondY,
                second.getRadius());
        }

        if (first.getShape() == GEColliderShape::Aabb
            && second.getShape() == GEColliderShape::Aabb) {
            return aabbAabb(
                firstX,
                firstY,
                first.getHalfWidth(),
                first.getHalfHeight(),
                secondX,
                secondY,
                second.getHalfWidth(),
                second.getHalfHeight());
        }

        if (first.getShape() == GEColliderShape::Circle) {
            return circleAabb(
                firstX,
                firstY,
                first.getRadius(),
                secondX,
                secondY,
                second.getHalfWidth(),
                second.getHalfHeight());
        }

        return circleAabb(
            secondX,
            secondY,
            second.getRadius(),
            firstX,
            firstY,
            first.getHalfWidth(),
            first.getHalfHeight());
    }

    static bool overlaps(
        const GETransformComponent& firstTransform,
        const GEColliderComponent& first,
        const GETransformComponent& secondTransform,
        const GEColliderComponent& second) {
        return overlaps(
            firstTransform.getCenterX(),
            firstTransform.getCenterY(),
            first,
            secondTransform.getCenterX(),
            secondTransform.getCenterY(),
            second);
    }
};
