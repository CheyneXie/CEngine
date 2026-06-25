/**
 * @file Base.ixx
 * @brief 全局Base
 * @version 1.0
 * @author Chaim
 * @date 2024/10/09
 */

module;
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
export module CEngine.Base;
export import :Object;
export import :Event;
export import :Pixel;
import std;

namespace CEngine {
    export auto WorldZero = glm::vec3(0.0f, 0.0f, 0.0f);
    export auto WorldForward = glm::vec3(0.0f, 0.0f, 1.0f);
    export auto WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    export auto WorldRight = glm::vec3(1.0f, 0.0f, 0.0f);

    /// 单位：弧度！
    export struct EulerRotation {
        float Yaw = 0.0f;
        float Pitch = 0.0f;
        float Roll = 0.0f;

        static EulerRotation FromDegrees(const float yaw, const float pitch, const float roll) {
            return EulerRotation{glm::radians(yaw), glm::radians(pitch), glm::radians(roll)};
        }

        static EulerRotation FromDegreesXYZ(const float x, const float y, const float z) {
            return FromDegrees(y, x, z);
        }

        glm::quat ToOrientation() const {
            return glm::normalize(glm::angleAxis(Yaw, glm::vec3(0.0f, 1.0f, 0.0f)) *
                                  glm::angleAxis(Pitch, glm::vec3(1.0f, 0.0f, 0.0f)) *
                                  glm::angleAxis(Roll, glm::vec3(0.0f, 0.0f, 1.0f)));
        }

        glm::vec3 ToVec3XYZ() const {
            return {Pitch, Yaw, Roll};
        }

        glm::vec3 ToDegreesVec3XYZ() const {
            return {glm::degrees(Pitch), glm::degrees(Yaw), glm::degrees(Roll)};
        }

        glm::vec3 RotateVector(const glm::vec3&& vector) const {
            return ToOrientation() * vector;
        }
    };

    // 透明查找
    export struct StringHash {
        using is_transparent = void;
        size_t operator()(std::string_view sv) const noexcept {
            return std::hash<std::string_view>{}(sv);
        }
        size_t operator()(const std::string& s) const noexcept {
            return std::hash<std::string_view>{}(s);
        }
        size_t operator()(const char* cstr) const noexcept {
            return std::hash<std::string_view>{}(cstr);
        }
    };
    export struct StringEqual {
        using is_transparent = void;
        bool operator()(std::string_view a, std::string_view b) const noexcept {
            return a == b;
        }
    };
}
