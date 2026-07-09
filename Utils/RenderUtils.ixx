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
    /**
     * @brief 帧常量 FBO
     */
    struct FrameConstantsUBO {
        glm::vec4 CameraPosition;
        glm::vec4 LightDirection;
        glm::vec4 LightColorAndIntensity;
    };

    /**
     * @brief 上传帧常量
     * @param CamPos 摄像机位置
     * @param L 方向光对象
     * @return export 
     */
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

    /**
     * @brief 获取一个空的 VAO（无顶点缓冲）
     * @remark Core Profile 下 glDrawArrays 需要绑定 VAO。全屏三角形（由 gl_VertexID 生成顶点）
     *         无需任何顶点属性，故复用此空 VAO。惰性创建，引擎生命周期内不释放。
     */
    export unsigned int GetEmptyVAO() {
        static unsigned int vao = 0;
        if (vao == 0) glGenVertexArrays(1, &vao);
        return vao;
    }
}