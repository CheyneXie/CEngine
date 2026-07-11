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
import CEngine.Render;
import std;

namespace CEngine::Utils {
    /**
     * @brief 帧常量 FBO
     */
    struct FrameConstantsUBO {
        glm::vec4 CameraPosition;
        glm::vec4 LightDirection;
        glm::vec4 LightColorAndIntensity;
        float DiffuseIBLStrength;
        float SpecularIBLStrength;
    };

    /**
     * @brief 上传帧常量
     * @param CamPos 摄像机位置
     * @param L 方向光对象
     * @param IBLStrength IBL光照强度
     */
    export void UploadFrameConstantsUBO(glm::vec3& CamPos, Light::Directional *L, IBL *ibl) {
        if (!L) return;
        FrameConstantsUBO data = {
            {CamPos, 0},
            {L->getDirection(), 0},
            {L->getColor().ToVec3(), L->getIlluminance()},
            ibl ? ibl->getDiffuseIBLStrength() : 1,
            ibl ? ibl->getSpecularIBLStrength() : 1
        };

        static unsigned int ubo = 0;
        if (ubo == 0) glGenBuffers(1, &ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(FrameConstantsUBO), &data, GL_STREAM_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        constexpr unsigned int BINDING_POINT = 0;
        glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_POINT, ubo);
    }

    /**
     * @brief 上传帧数据，包括 FrameConstantsUBO、IBL
     * @param CamPos 相机位置
     * @param L 方向光对象
     * @param atmosphere 天空盒对象
     */
    export void UploadFrameConstantsData(glm::vec3& CamPos, Light::Directional *L) {
        auto ibl = IBL::GetActive();
        // TODO 设置高位 IBL 纹理
        UploadFrameConstantsUBO(CamPos, L, ibl);
    }
}