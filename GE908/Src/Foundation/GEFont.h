#pragma once
#include "../../ThirdParty/GamesEngineeringBase.h"
#include "GEUtility.h"
using namespace GamesEngineeringBase;

class GEFont {
private:
    Image* _fontChar[95] = { nullptr }; // 32~126 ASCII
    bool _isLoaded = false;

public:
    GEFont() = default;
    ~GEFont() { release(); }

    void release() {
        for (int i = 0; i < 95; ++i) {
            delete _fontChar[i];
            _fontChar[i] = nullptr;
        }
        _isLoaded = false;
    }

    bool load(const std::string& folder = "Src/Assets/Fonts/") {
        for (int i = 0; i < 95; ++i) {
            std::string filename = folder + std::to_string(i + 32) + ".png";
            Image* img = new Image();
            if (!img->load(filename)) {
                delete img;
                for (int j = 0; j < i; ++j) {
                    delete _fontChar[j];
                    _fontChar[j] = nullptr;
                }
                return false;
            }
            _fontChar[i] = img;
        }
        _isLoaded = true;
        return true;
    }

    Image* getChar(char c) const {
        if (!_isLoaded || c < 32 || c > 126) return nullptr;
        return _fontChar[c - 32];
    }

    GESize draw(const std::string& text, GEPoint originPoint, const GEColor textColor, Window& window) const {
        if (!_isLoaded) return GESize(0, 0);
        int cursorX = static_cast<int>(originPoint.x);
        int height = 0;

        int windowWidth = window.getWidth();
        int windowHeight = window.getHeight();

        for (char c : text) {
            Image* font = getChar(c);
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
