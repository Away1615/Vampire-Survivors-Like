#pragma once

#include "../../Foundation/GEConst.h"

struct GEPlayerAoeImpact {
    float x = 0.0f;
    float y = 0.0f;
    float remainingTime = 0.0f;
};

class GEPlayerComponent {
private:
    float _autoAttackTimer = 0.0f;
    float _autoAttackIntervalBase = Player::PLAYER_BASE_AUTO_ATTACK_INTERVAL;
    float _autoAttackSpeedMultiplier = 1.0f;
    float _attackSpeedBuffTimer = 0.0f;

    float _aoeCooldownTimer = 0.0f;
    float _aoeCooldown = 5.0f;
    float _aoeRadius = 300.0f;
    float _aoeEffectDuration = 0.35f;
    int _aoeTargetCount = Player::PLAYER_BASE_AOE_TARGETS;
    float _aoeTargetBuffTimer = 0.0f;
    bool _aoeRequested = false;

    GEPlayerAoeImpact _aoeImpacts[Player::PLAYER_MAX_AOE_EFFECTS];
    int _aoeImpactCount = 0;

    bool _wasInFire = false;
    float _fireTimer = 0.0f;

public:
    GEPlayerComponent() {
        reset();
    }

    void reset() {
        _autoAttackTimer = 0.0f;
        _autoAttackIntervalBase = Player::PLAYER_BASE_AUTO_ATTACK_INTERVAL;
        _autoAttackSpeedMultiplier = 1.0f;
        _attackSpeedBuffTimer = 0.0f;
        _aoeCooldownTimer = 0.0f;
        _aoeCooldown = 5.0f;
        _aoeRadius = 300.0f;
        _aoeEffectDuration = 0.35f;
        _aoeTargetCount = Player::PLAYER_BASE_AOE_TARGETS;
        _aoeTargetBuffTimer = 0.0f;
        _aoeRequested = false;
        _aoeImpactCount = 0;
        _wasInFire = false;
        _fireTimer = 0.0f;
    }

    void updateBuffs(float deltaTime) {
        if (_attackSpeedBuffTimer > 0.0f) {
            _autoAttackSpeedMultiplier = Player::PLAYER_BUFF_ATTACK_SPEED_MULTIPLIER;
            _attackSpeedBuffTimer = _attackSpeedBuffTimer > deltaTime
                ? _attackSpeedBuffTimer - deltaTime
                : 0.0f;
            if (_attackSpeedBuffTimer <= 0.0f) {
                _autoAttackSpeedMultiplier = 1.0f;
            }
        }

        if (_aoeTargetBuffTimer > 0.0f) {
            _aoeTargetCount = Player::PLAYER_MAX_AOE_TARGETS;
            _aoeTargetBuffTimer = _aoeTargetBuffTimer > deltaTime
                ? _aoeTargetBuffTimer - deltaTime
                : 0.0f;
            if (_aoeTargetBuffTimer <= 0.0f) {
                _aoeTargetCount = Player::PLAYER_BASE_AOE_TARGETS;
            }
        }
    }

    bool advanceAutoAttack(float deltaTime) {
        _autoAttackTimer += deltaTime;
        const float interval = _autoAttackIntervalBase / _autoAttackSpeedMultiplier;
        if (_autoAttackTimer < interval) return false;
        _autoAttackTimer -= interval;
        return true;
    }

    void updateAoe(float deltaTime) {
        int index = 0;
        while (index < _aoeImpactCount) {
            _aoeImpacts[index].remainingTime -= deltaTime;
            if (_aoeImpacts[index].remainingTime > 0.0f) {
                ++index;
                continue;
            }
            for (int i = index; i < _aoeImpactCount - 1; ++i) {
                _aoeImpacts[i] = _aoeImpacts[i + 1];
            }
            --_aoeImpactCount;
        }

        if (_aoeCooldownTimer > 0.0f) {
            _aoeCooldownTimer = _aoeCooldownTimer > deltaTime
                ? _aoeCooldownTimer - deltaTime
                : 0.0f;
        }
    }

    void recordAoeImpact(float centerX, float centerY) {
        if (_aoeImpactCount >= Player::PLAYER_MAX_AOE_EFFECTS) {
            for (int i = 1; i < _aoeImpactCount; ++i) {
                _aoeImpacts[i - 1] = _aoeImpacts[i];
            }
            --_aoeImpactCount;
        }

        GEPlayerAoeImpact& impact = _aoeImpacts[_aoeImpactCount++];
        impact.x = centerX;
        impact.y = centerY;
        impact.remainingTime = _aoeEffectDuration;
    }

    void restore(
        float autoAttackTimer,
        float autoAttackSpeedMultiplier,
        float attackSpeedBuffTimer,
        float aoeCooldownTimer,
        float aoeCooldown,
        int aoeTargetCount,
        bool aoeRequested,
        float aoeTargetBuffTimer) {
        _autoAttackTimer = autoAttackTimer;
        _autoAttackSpeedMultiplier = autoAttackSpeedMultiplier > 0.1f
            ? autoAttackSpeedMultiplier
            : 0.1f;
        _attackSpeedBuffTimer = attackSpeedBuffTimer > 0.0f ? attackSpeedBuffTimer : 0.0f;
        _aoeCooldownTimer = aoeCooldownTimer;
        if (aoeCooldown > 0.0f) {
            _aoeCooldown = aoeCooldown;
        }
        if (aoeTargetCount > 0) {
            _aoeTargetCount = aoeTargetCount < Player::PLAYER_MAX_AOE_TARGETS
                ? aoeTargetCount
                : Player::PLAYER_MAX_AOE_TARGETS;
        }
        else {
            _aoeTargetCount = Player::PLAYER_BASE_AOE_TARGETS;
        }
        _aoeRequested = aoeRequested;
        _aoeTargetBuffTimer = aoeTargetBuffTimer > 0.0f ? aoeTargetBuffTimer : 0.0f;

        _autoAttackSpeedMultiplier = _attackSpeedBuffTimer > 0.0f
            ? Player::PLAYER_BUFF_ATTACK_SPEED_MULTIPLIER
            : 1.0f;
        _aoeTargetCount = _aoeTargetBuffTimer > 0.0f
            ? Player::PLAYER_MAX_AOE_TARGETS
            : Player::PLAYER_BASE_AOE_TARGETS;
    }

    void applyAttackSpeedBoost() {
        _autoAttackSpeedMultiplier = Player::PLAYER_BUFF_ATTACK_SPEED_MULTIPLIER;
        _attackSpeedBuffTimer = Player::PLAYER_POWERUP_DURATION_SECONDS;
    }

    void applyAdditionalAoeTarget() {
        _aoeTargetCount = Player::PLAYER_MAX_AOE_TARGETS;
        _aoeTargetBuffTimer = Player::PLAYER_POWERUP_DURATION_SECONDS;
    }

    float getAutoAttackTimer() const { return _autoAttackTimer; }
    float getAutoAttackSpeedMultiplier() const { return _autoAttackSpeedMultiplier; }
    float getAttackSpeedBuffTimer() const { return _attackSpeedBuffTimer; }
    float getAoeCooldownTimer() const { return _aoeCooldownTimer; }
    float getAoeCooldown() const { return _aoeCooldown; }
    float getAoeRadius() const { return _aoeRadius; }
    int getAoeTargetCount() const { return _aoeTargetCount; }
    float getAoeTargetBuffTimer() const { return _aoeTargetBuffTimer; }

    void requestAoe() { _aoeRequested = true; }
    bool isAoeRequested() const { return _aoeRequested; }
    void clearAoeRequest() { _aoeRequested = false; }
    void startAoeCooldown() { _aoeCooldownTimer = _aoeCooldown; }

    int getAoeImpactCount() const { return _aoeImpactCount; }
    const GEPlayerAoeImpact& getAoeImpact(int index) const { return _aoeImpacts[index]; }

    bool wasInFire() const { return _wasInFire; }
    void setWasInFire(bool wasInFire) { _wasInFire = wasInFire; }
    float getFireTimer() const { return _fireTimer; }
    void setFireTimer(float fireTimer) { _fireTimer = fireTimer; }
};
