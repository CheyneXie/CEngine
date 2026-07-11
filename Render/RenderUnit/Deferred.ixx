/**
 * @file Deferred.ixx
 * @author Cheyne Xie
 * @brief 延迟渲染单元
 * @version 1.0
 * @date 2026-06-20
 *
 * @remark 几何通道写 GBuffer；光照通道读 GBuffer + IBL，输出到 HDR SceneFBO（线性 HDR）。
 * @remark 几何通道后把 GBuffer 深度 blit 到 SceneFBO，使前向物体与延迟几何正确深度遮挡。
 */

module;
#include <glm/glm.hpp>
#include <glad/glad.h>
export module CEngine.RenderUnit:Deferred;
import :Base;
import :GBuffer;
import :SceneFBO;
import CEngine.Render;
import CEngine.Light;
import CEngine.Utils.RenderUtils;

namespace CEngine::RenderUnit {
    export class Deferred : public Base {
    public:
        Type GetType() override { return Type::Deferred_PBR; }

        static std::unique_ptr<Deferred> Create(std::shared_ptr<Mesh> m, Material &&mat) {
            return std::unique_ptr<Deferred>(new Deferred(std::move(m), std::move(mat)));
        }

        /**
         * @brief 批渲染：几何通道 -> blit 深度 -> 光照通道
         *
         * @param RUS
         * @param viewM 视图矩阵
         * @param projectM 投影矩阵
         * @param pointLightCount 点光数量
         */
        static void RenderAll(std::vector<Base*>& RUS, const glm::mat4 &viewM, const glm::mat4 &projectM, int pointLightCount) {
            // 写 GBuffer
            GBuffer().Bind();
            auto sp_geo = RenderUtil_GetShaderProgramWithBasicsData_Geometry(viewM, projectM);
            for (auto ru : RUS) {
                static_cast<Deferred*>(ru)->RenderGeometry(viewM, projectM, sp_geo);
            }
            // 把 GBuffer 深度 blit 到 SceneFBO
            auto [w, h] = GBuffer().GetSize();
            glBindFramebuffer(GL_READ_FRAMEBUFFER, GBuffer().getFBO());
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, SceneFBO().getFBO());
            glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

            SceneFBO::Get()->BindNoClear();
            Texture::ResetTextureSlot();
            RenderLighting(pointLightCount);
        }

        /**
         * @brief 执行渲染：几何阶段
         *
         * @param viewM 视图矩阵
         * @param projectM 透视矩阵
         * @param sp 着色器程序指针
         * @remark 延迟渲染尽量不要单独调用渲染函数
         */
        void RenderGeometry(const glm::mat4 &viewM, const glm::mat4 &projectM, ShaderProgram *sp = nullptr) {
            #ifndef NDEBUG
            if (GetType() != Type::Deferred_PBR) LogW("Deferred") << "派生类调用了父类的 Render 函数";
            #endif
            Texture::ResetTextureSlot();
            if (sp == nullptr)
                sp = RenderUtil_GetShaderProgramWithBasicsData_Geometry(viewM, projectM);
            sp->SetUniform(0, WorldMatrix);
            Mat.Use(sp);
            RenderUtil_SetShaderUniform(sp);
            mesh->Render();
            RenderUtil_ResetShaderUniform(sp);
        }

        /**
         * @brief 执行渲染：灯光阶段
         * @param pointLightCount 点光数量
         */
        static void RenderLighting(int pointLightCount) {
            auto sp = ShaderProgram::Get("Deferred-Lighting");
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
            sp->Use();
            Texture::ResetTextureSlot();
            GBuffer().Use(sp);
            
            // irradiance/prefilter/brdfLUT -> 12/13/14
            if (auto *ibl = IBL::GetActive()) ibl->Bind(sp);
            sp->SetUniform("PointLightCount", pointLightCount);

            glBindVertexArray(GetEmptyVAO());
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);

            glDepthMask(GL_TRUE);
            glEnable(GL_DEPTH_TEST);
        }

        Material &getMaterial() {
            return Mat;
        }

    protected:
        /**
         * @brief 工具函数：获取 Deferred-Geometry 着色器程序并设置基础数据
         */
        static ShaderProgram *RenderUtil_GetShaderProgramWithBasicsData_Geometry(const glm::mat4 &viewM, const glm::mat4 &projectM) {
            auto sp = ShaderProgram::Get("Deferred-Geometry");
            sp->Use();
            sp->SetUniform(1, viewM);
            sp->SetUniform(2, projectM);
            return sp;
        }


        Deferred(std::shared_ptr<Mesh> m, Material &&mat) : Base(std::move(m)), Mat(std::move(mat)) {
            shader_program_names.push_back("Deferred-Geometry");
            shader_program_names.push_back("Deferred-Lighting");
        }

        Material Mat;
    };
}
