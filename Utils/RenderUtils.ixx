/**
 * @file RenderUtils.ixx
 * @author Cheyne Xie
 * @brief 一些渲染相关的工具函数
 * @version 1.0
 * @date 2026-06-25
 * 
 */

module;
#include <glad/glad.h>
#include <glm/glm.hpp>
export module CEngine.Utils.RenderUtils;
import CEngine.Light;
import std;

namespace CEngine::Utils {
    struct FrameConstantsUBO {
        glm::vec4 CameraPosition;
        glm::vec4 LightDirection;
        glm::vec4 LightColorAndIntensity;
    };
    export void UploadFrameConstantsUBO(glm::vec3& CamPos, Light::Directional *L) {
        if (!L) return;
        FrameConstantsUBO data = {
            {CamPos, 0},
            {L->getDirection(), 0},
            {L->getColor().ToVec3(), L->getIlluminance()}
        };

        static unsigned int ubo = 0;
        if (ubo == 0) glGenBuffers(1, &ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(FrameConstantsUBO), &data, GL_STREAM_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        constexpr unsigned int BINDING_POINT = 0;
        glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_POINT, ubo);
    }

}