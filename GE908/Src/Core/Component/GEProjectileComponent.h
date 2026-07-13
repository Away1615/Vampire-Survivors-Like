#pragma once

#include "../../Foundation/GEModel.h"

class GEProjectileComponent {
private:
    float _speed = 0.0f;
    float _dirX = 0.0f;
    float _dirY = 0.0f;
    int _damage = 0;
    bool _active = false;
    ProjectileOwner _owner = ProjectileOwner::FromPlayer;

public:
    void spawn(ProjectileOwner owner, float dirX, float dirY, float speed, int damage) {
        _owner = owner;
        _dirX = dirX;
        _dirY = dirY;
        _speed = speed;
        _damage = damage;
        _active = true;
    }

    void deactivate() { _active = false; }

    bool isActive() const { return _active; }
    ProjectileOwner getOwner() const { return _owner; }
    float getSpeed() const { return _speed; }
    float getDirectionX() const { return _dirX; }
    float getDirectionY() const { return _dirY; }
    int getDamage() const { return _damage; }
};
