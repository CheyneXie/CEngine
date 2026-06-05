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
import std;

namespace CEngine {
    // export using shader_uniforms_type = std::variant<int, int *, unsigned int, unsigned int *, float, float *, double, double *, glm::mat3, glm::mat4>;
    // export using shader_uniforms_var = std::variant<int, unsigned int, float>;

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
    };
}
