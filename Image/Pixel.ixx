/**
 * @file Pixel.ixx
 * @brief 像素点类
 * @version 1.0
 * @author Chaim
 * @date 2024/09/29
 */

module;
#include <string.h>
export module CEngine.Image:Pixel;
import std;
import CEngine.Utils;
import CEngine.Logger;

namespace CEngine {
    export enum class ColorMode {
        NONE,
        GRAY,
        GRAY_A,
        RGB,
        RGBA
    };

    export int ColorMode_GetChannelCount(const ColorMode mode) {
        switch (mode) {
            case ColorMode::GRAY:
                return 1;
            case ColorMode::GRAY_A:
                return 2;
            case ColorMode::RGB:
                return 3;
            case ColorMode::RGBA:
                return 4;
            default: return 0;
        }
    }

    export struct RGB {
        unsigned char R;
        unsigned char G;
        unsigned char B;

        RGB() : R(0), G(0), B(0) {
        };

        RGB(const unsigned char r, const unsigned char g, const unsigned char b) : R(r), G(g), B(b) {
        };
        explicit RGB(const char *hex_color);
        std::string ToString() const;

    private:
        const static char *TAG;
    };

    export struct RGBA {
        unsigned char R;
        unsigned char G;
        unsigned char B;
        unsigned char A;

        RGBA() : R(0), G(0), B(0), A(255) {
        };

        RGBA(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a) : R(r), G(g), B(b), A(a) {
        };

        explicit RGBA(const char *hex_color);

        RGBA(const RGB rgb, const unsigned char a) : R(rgb.R), G(rgb.G), B(rgb.G), A(a) {
        };

        RGB ToRGB() const { return {R, G, B}; };

        std::string ToString() const;

    private:
        const static char *TAG;
    };

    export struct GRAY {
        unsigned char Value;

        GRAY() : Value(0) {
        };

        explicit GRAY(const unsigned char value) : Value(value) {
        };

        std::string ToString() const;

    private:
        const static char *TAG;
    };

    export struct GRAY_A {
        unsigned char Value;
        unsigned char Alpha;

        GRAY_A() : Value(0), Alpha(255) {
        };

        GRAY_A(const unsigned char value, const unsigned char alpha) : Value(value), Alpha(alpha) {
        };

        std::string ToString() const;

    private:
        const static char *TAG;
    };

    struct HSV {
        // TODO
    };
}


namespace CEngine {
    const std::regex HexColorPattern(R"(^#([A-Fa-f0-9]{6})([A-Fa-f0-9]{2})?$)");

    const char *RGB::TAG = "RGB";

    RGB::RGB(const char *hex_color) {
        if (std::regex_match(hex_color, HexColorPattern)) {
            R = Utils::HexToChar(hex_color[1]) * 16 + Utils::HexToChar(hex_color[2]);
            G = Utils::HexToChar(hex_color[3]) * 16 + Utils::HexToChar(hex_color[4]);
            B = Utils::HexToChar(hex_color[5]) * 16 + Utils::HexToChar(hex_color[6]);
        } else {
            LogE(TAG) << "十六进制颜色解析错误: " << hex_color;
            new(this) RGB();
        }
    }

    std::string RGB::ToString() const {
        std::stringstream ss;
        ss << "RGB(" << static_cast<int>(R) << "," << static_cast<int>(G) << "," << static_cast<int>(B) << ")";
        return ss.str();
    }

    const char *RGBA::TAG = "RGBA";

    RGBA::RGBA(const char *hex_color) {
        if (std::regex_match(hex_color, HexColorPattern)) {
            R = Utils::HexToChar(hex_color[1]) * 16 + Utils::HexToChar(hex_color[2]);
            G = Utils::HexToChar(hex_color[3]) * 16 + Utils::HexToChar(hex_color[4]);
            B = Utils::HexToChar(hex_color[5]) * 16 + Utils::HexToChar(hex_color[6]);
            if (strlen(hex_color) == 9)
                A = Utils::HexToChar(hex_color[7]) * 16 + Utils::HexToChar(hex_color[8]);
            else
                A = 255;
        } else {
            LogE(TAG) << "十六进制颜色解析错误: " << hex_color;
            new(this) RGBA();
        }
    }

    std::string RGBA::ToString() const {
        std::stringstream ss;
        ss << "RGBA(" << static_cast<int>(R) << "," << static_cast<int>(G) << "," << static_cast<int>(B) << "," <<
                static_cast<int>(A) << ")";
        return ss.str();
    }

    const char *GRAY::TAG = "灰度";

    std::string GRAY::ToString() const {
        std::stringstream ss;
        ss << "灰度(" << static_cast<int>(Value) << ")";
        return ss.str();
    }

    const char *GRAY_A::TAG = "灰度&Alpha";

    std::string GRAY_A::ToString() const {
        std::stringstream ss;
        ss << "灰度&Alpha(" << static_cast<int>(Value) << "," << static_cast<int>(Alpha) << ")";
        return ss.str();
    }
}
