/**
 * @file Pixel.ixx
 * @brief 像素点类
 * @version 2.0
 * @author Chaim
 * @date 2024/09/29
 */

module;
#include <string.h>
#include <glm/glm.hpp>
export module CEngine.Base:Pixel;
import std;
import CEngine.Utils;
import CEngine.Logger;

namespace CEngine {
    const std::regex HexColorPattern(R"(^#([A-Fa-f0-9]{6})([A-Fa-f0-9]{2})?$)");

    export enum class ColorMode {
        NONE,
        Gray,
        GrayA,
        RGB,
        RGBA
    };

    export int ColorMode_GetChannelCount(const ColorMode mode) {
        switch (mode) {
            case ColorMode::Gray:
                return 1;
            case ColorMode::GrayA:
                return 2;
            case ColorMode::RGB:
                return 3;
            case ColorMode::RGBA:
                return 4;
            default: return 0;
        }
    }

    export struct Pixel {
    };

    export struct RGB;
    export struct RGBA;
    export struct Gray;
    export struct GrayA;
}

/* ================================================================= */
namespace CEngine {
    struct RGB : public Pixel {
        float R;
        float G;
        float B;

        RGB() : R(0), G(0), B(0) {
        };

        RGB(const float r, const float g, const float b) : R(r), G(g), B(b) {
        };

        RGB(const unsigned char r, const unsigned char g, const unsigned char b) {
            R = static_cast<float>(r) / 255.f;
            G = static_cast<float>(g) / 255.f;
            B = static_cast<float>(b) / 255.f;
        };

        RGB(const unsigned short r, const unsigned short g, const unsigned short b) {
            R = static_cast<float>(r) / 65535.f;
            G = static_cast<float>(g) / 65535.f;
            B = static_cast<float>(b) / 65535.f;
        };

        explicit RGB(const char *hex_color) {
            if (std::regex_match(hex_color, HexColorPattern)) {
                R = Utils::HexToFloat(hex_color[1], hex_color[2]);
                G = Utils::HexToFloat(hex_color[3], hex_color[4]);
                B = Utils::HexToFloat(hex_color[5], hex_color[6]);
            } else {
                LogE(TAG) << "十六进制颜色解析错误: " << hex_color;
                R = 0;
                G = 0;
                B = 0;
            }
        }

        RGBA ToRGBA() const;
        Gray ToGray() const;
        GrayA ToGrayA() const;

        glm::vec3 ToVec3() const { return glm::vec3(R, G, B); };

        std::string ToString() const {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(4);
            ss << "RGB(" << R << "," << G << "," << B << ")";
            return ss.str();
        }

    private:
        const static char *TAG;
    };
    const char *RGB::TAG = "RGB";
}

/* ================================================================= */
namespace CEngine {
    struct RGBA : public Pixel  {
        float R;
        float G;
        float B;
        float A;

        RGBA() : R(0), G(0), B(0), A(1.f) {
        };

        RGBA(const float r, const float g, const float b, const float a) : R(r), G(g), B(b), A(a) {
        };

        RGBA(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a) {
            R = static_cast<float>(r) / 255.f;
            G = static_cast<float>(g) / 255.f;
            B = static_cast<float>(b) / 255.f;
            A = static_cast<float>(a) / 255.f;
        };

        RGBA(const unsigned short r, const unsigned short g, const unsigned short b, const unsigned short a) {
            R = static_cast<float>(r) / 65535.f;
            G = static_cast<float>(g) / 65535.f;
            B = static_cast<float>(b) / 65535.f;
            A = static_cast<float>(a) / 65535.f;
        };

        RGBA(const RGB rgb, const unsigned char a) : R(rgb.R), G(rgb.G), B(rgb.G), A(a) {
        };

        explicit RGBA(const char *hex_color) {
            if (std::regex_match(hex_color, HexColorPattern)) {
                R = Utils::HexToFloat(hex_color[1], hex_color[2]);
                G = Utils::HexToFloat(hex_color[3], hex_color[4]);
                B = Utils::HexToFloat(hex_color[5], hex_color[6]);
                if (strlen(hex_color) == 9) A = Utils::HexToFloat(hex_color[7], hex_color[8]);
                else A = 1.f;
            } else {
                LogE(TAG) << "十六进制颜色解析错误: " << hex_color;
                R = 0;
                G = 0;
                B = 0;
                A = 1.f;
            }
        }

        RGB ToRGB() const;
        Gray ToGray() const;
        GrayA ToGrayA() const;

        glm::vec4 ToVec4() const { return {R, G, B, A}; }

        std::string ToString() const {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(4);
            ss << "RGBA(" << R << "," << G << "," << B << "," << A << ")";
            return ss.str();
        }

    private:
        const static char *TAG;
    };
    const char *RGBA::TAG = "RGBA";
}

/* ================================================================= */
namespace CEngine {
    struct Gray : public Pixel  {
        float Value;

        Gray() : Value(0) {
        };

        Gray(const float value) : Value(value) {
        }

        explicit Gray(const unsigned char value) : Value(static_cast<float>(value) / 255.f) {
        };
        explicit Gray(const unsigned short value) : Value(static_cast<float>(value) / 65535.f) {
        };

        RGB ToRGB() const;
        RGBA ToRGBA() const;
        GrayA ToGrayA() const;

        std::string ToString() const {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(4);
            ss << "Gray(" << Value << ")";
            return ss.str();
        }

    private:
        const static char *TAG;
    };
    const char *Gray::TAG = "灰度";
}

/* ================================================================= */
namespace CEngine {
    struct GrayA : public Pixel  {
        float Value;
        float Alpha;

        GrayA() : Value(0), Alpha(1.f) {
        };

        GrayA(const float value, const float alpha) : Value(value), Alpha(alpha) {
        }

        GrayA(const unsigned char value, const unsigned char alpha) {
            Value = static_cast<float>(value) / 255.f;
            Alpha = static_cast<float>(alpha) / 255.f;
        };
        GrayA(const unsigned short value, const unsigned short alpha) {
            Value = static_cast<float>(value) / 65535.f;
            Alpha = static_cast<float>(alpha) / 65535.f;
        };

        RGB ToRGB() const;
        RGBA ToRGBA() const;
        Gray ToGray() const;

        glm::vec2 ToVec2() const { return {Value, Alpha}; };

        std::string ToString() const {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(4);
            ss << "GrayA(" << Value << "," << Alpha << ")";
            return ss.str();
        }

    private:
        const static char *TAG;
    };
    const char *GrayA::TAG = "灰度&Alpha";
}

/* ================================================================= */
namespace CEngine {
    struct HSV {
        // TODO
    };
}

/* ================================================================= */
namespace CEngine {
    RGBA RGB::ToRGBA() const { return {R, G, B, 1.f}; }
    Gray RGB::ToGray() const { return {0.299f * R + 0.587f * G + 0.114f * B}; }
    GrayA RGB::ToGrayA() const { return {0.299f * R + 0.587f * G + 0.114f * B, 1.f}; }

    RGB RGBA::ToRGB() const { return {R, G, B}; }
    Gray RGBA::ToGray() const { return {0.299f * R + 0.587f * G + 0.114f * B}; }
    GrayA RGBA::ToGrayA() const { return {0.299f * R + 0.587f * G + 0.114f * B, A}; }

    RGB Gray::ToRGB() const { return {Value, Value, Value}; }
    RGBA Gray::ToRGBA() const { return {Value, Value, Value, 1.f}; }
    GrayA Gray::ToGrayA() const { return {Value, 1.f}; }

    RGB GrayA::ToRGB() const { return {Value, Value, Value}; }
    RGBA GrayA::ToRGBA() const { return {Value, Value, Value, Alpha}; }
    Gray GrayA::ToGray() const { return {Value}; }
}