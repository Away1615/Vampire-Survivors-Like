#pragma once

#include "../../Foundation/GEModel.h"

class GEEnemyComponent {
private:
    GEEnemyType _type = GEEnemyType::Normal;
    float _attackCooldown = 0.0f;
    float _attackRate = 2.0f;
    bool _stationary = false;

public:
    explicit GEEnemyComponent(GEEnemyType type = GEEnemyType::Normal) {
        configure(type);
    }

    void configure(GEEnemyType type) {
        _type = type;
        _attackCooldown = 0.0f;
        _attackRate = 2.0f;
        _stationary = false;

        if (type == GEEnemyType::StaticShooter) {
            _attackRate = 2.8f;
            _stationary = true;
        }
    }

    GEEnemyType getType() const { return _type; }
    float getAttackCooldown() const { return _attackCooldown; }
    float getAttackRate() const { return _attackRate; }
    bool isStationary() const { return _stationary; }

    void setAttackCooldown(float attackCooldown) { _attackCooldown = attackCooldown; }
};
