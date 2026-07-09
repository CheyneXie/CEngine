/**
 * @file Postprocessing.ixx
 * @author Cheyne Xie
 * @brief 全屏后处理
 * @version 1.0
 * @date 2026-07-01
 *
 */

module;
#include <glad/glad.h>
export module CEngine.RenderUnit:Postprocessing;
import :SceneFBO;
import CEngine.Render;
import CEngine.Utils.RenderUtils;
import CEngine.Logger;

namespace CEngine::RenderUnit {
    export class Postprocessing final : public Object {
    public:
        static const char *TAG;

        /**
         * @brief 全屏后处理
         */
        static void RenderAll() {
            auto *sp = ShaderProgram::Get("Postprocess");
            // 切换默认帧缓冲
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);

            sp->Use();
            SceneFBO().Use(sp); // 绑定 SceneColor

            glBindVertexArray(Utils::GetEmptyVAO());
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);

            glDepthMask(GL_TRUE);
            glEnable(GL_DEPTH_TEST);
        }
    };

    const char *Postprocessing::TAG = "Postprocessing";
}
