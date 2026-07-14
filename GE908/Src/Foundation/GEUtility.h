#pragma once
#include <cstdint>

static float clamp(float value, float minVal, float maxVal) {
	if (value < minVal) return minVal;
	if (value > maxVal) return maxVal;
	return value;
}

static uint32_t nextRandom(uint32_t& state) {
    state = state * 1664525u + 1013904223u;
    return state;
}

static int randomIndex(uint32_t& state, int upperBound) {
    return static_cast<int>(nextRandom(state) % static_cast<uint32_t>(upperBound));
}

static float randomFloat(uint32_t& state, float from = 0.0f, float to = 1.0f) {
    const float unit = static_cast<float>(nextRandom(state) & 0x00FFFFFFu) / 16777216.0f;
    return from + (to - from) * unit;
}


struct GEColor {
    unsigned char r = 255;
    unsigned char g = 255;
    unsigned char b = 255;

    GEColor() = default;
    GEColor(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {}
};

static const GEColor RED = GEColor(255, 0, 0);
static const GEColor GREEN = GEColor(0, 255, 0);
static const GEColor BLUE = GEColor(0, 0, 255);
static const GEColor WHITE = GEColor(255, 255, 255);

struct GEPoint {
    float x = 0.0f;
    float y = 0.0f;

    GEPoint(float x_, float y_) : x(x_), y(y_) {}
    GEPoint(int x_, int y_) : x(static_cast<float>(x_)), y(static_cast<float>(y_)) {}
};

struct GESize {
    float width = 0.0f;
    float height = 0.0f;

    GESize(float width_, float height_) : width(width_), height(height_) {}
    GESize(int width_, int height_) : width(static_cast<float>(width_)), height(static_cast<float>(height_)) {}
};
