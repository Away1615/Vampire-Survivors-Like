#pragma once
#include "../../ThirdParty/GamesEngineeringBase.h"
#include "GEUtility.h"
#include <array>
using namespace GamesEngineeringBase;

class GEFont {
private:
    std::array<Image, 95> _fontChars; // Printable ASCII range.
    bool _isLoaded = false;

public:
    GEFont() = default;
    ~GEFont() { release(); }

    void release() {
        for (Image& image : _fontChars) image.free();
        _isLoaded = false;
    }

    bool load(const std::string& folder) {
        release();
        for (int i = 0; i < 95; ++i) {
            std::string filename = folder + std::to_string(i + 32) + ".png";
            if (!_fontChars[static_cast<size_t>(i)].load(filename)) {
                release();
                return false;
            }
        }
        _isLoaded = true;
        return true;
    }

    const Image* getChar(char c) const {
        if (!_isLoaded || c < 32 || c > 126) return nullptr;
        return &_fontChars[static_cast<size_t>(c - 32)];
    }

    GESize draw(const std::string& text, GEPoint originPoint, const GEColor textColor, Window& window) const {
        if (!_isLoaded) return GESize(0, 0);
        int cursorX = static_cast<int>(originPoint.x);
        int height = 0;

        int windowWidth = window.getWidth();
        int windowHeight = window.getHeight();

        for (char c : text) {
            const Image* font = getChar(c);
            if (!font) continue;

            int imageWidth = font->width;
            int imageHeight = font->height;
            height = imageHeight;

            for (int dy = 0; dy < imageWidth; ++dy) {
                int screenY = static_cast<int>(originPoint.y) + dy;
                if (screenY < 0 || screenY >= windowHeight) continue;
                for (int dx = 0; dx < imageHeight; ++dx) {
                    int screenX = cursorX + dx;
                    if (screenX < 0 || screenX >= windowWidth) continue;

                    if (font->alphaAtUnchecked(dx, dy) > 0)
                        window.draw(screenX, screenY, textColor.r, textColor.g, textColor.b);
                }
            }
            cursorX += imageWidth;
        }

        return GESize(cursorX, height);
    }
};
