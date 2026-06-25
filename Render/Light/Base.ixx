/**
 * @file Base.ixx
 * @author Cheyne Xie
 * @brief 灯光基础组件
 * @version 1.0
 * @date 2026-06-24
 * 
 */


module;
#include <glm/glm.hpp>
export module CEngine.Light:Base;
import :Type;
import CEngine.Base;

namespace CEngine::Light {
    export class Base : public Object {
    public:
        virtual Light::Type GetType() = 0;

        Base(const Base&) = delete;
        Base& operator=(const Base&) = delete;
        Base(Base&&) = default;
        Base& operator=(Base&&) = default;

        float getIlluminance() { return Illuminance; }
        void setIlluminance(float lx) { Illuminance = lx; }

        RGB getColor() { return Color; }
        void setColor(const unsigned char r, const unsigned char g, const unsigned char b) {
            Color.R = r;
            Color.G = g;
            Color.B = b;
        }
        void setColor(RGB&& color) { setColor(color.R, color.G, color.B); }

    protected:
        Base() = default;

        /**
         * @brief 照度（单位: lx）
         */
        float Illuminance = 5;

        /**
         * @brief 颜色
         */
        RGB Color = {255, 255, 255};
    };
}