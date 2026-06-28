/**
 * @file Deferred.ixx
 * @author Cheyne Xie
 * @brief 延迟渲染单元
 * @version 1.0
 * @date 2026-06-20
 * 
 */

 module;
#include <glm/glm.hpp>
#include <glad/glad.h>
export module CEngine.RenderUnit:Deferred;
import :Base;
import :GBuffer;
import CEngine.Render;
import CEngine.Light;

namespace CEngine::RenderUnit {
    export class Deferred : public Base {
    public:
        Type GetType() override { return Type::Deferred_PBR; }

        static std::unique_ptr<Deferred> Create(std::shared_ptr<Mesh> m, Material &&mat) {
            return std::unique_ptr<Deferred>(new Deferred(std::move(m), std::move(mat)));
        }

        /**
         * @brief 批渲染
         * 
         * @param RUS 
         * @param viewM 视图矩阵
         * @param projectM 投影矩阵
         */
        static void RenderAll(std::vector<Base*>& RUS, const glm::mat4 &viewM, const glm::mat4 &projectM) {
            GBuffer().Bind();
            auto sp_geo = RenderUtil_GetShaderProgramWithBasicsData_Geometry(viewM, projectM);
            for (auto ru : RUS) {
                static_cast<Deferred*>(ru)->RenderGeometry(viewM, projectM, sp_geo);
                Texture::ResetTextureSlot();
            }
            GBuffer().Unbind();
            // RenderLighting(viewM, projectM);
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
            if (sp == nullptr)
                sp = RenderUtil_GetShaderProgramWithBasicsData_Geometry(viewM, projectM);
            sp->SetUniform(0, WorldMatrix);
            Mat.Use(sp);
            RenderUtil_SetShaderUniform(sp);
            mesh->Render();
            RenderUtil_ResetShaderUniform(sp);
        }

        /**
         * @brief 工具函数：获取 Deferred-Geometry 着色器程序并设置基础数据
         * 
         * @param viewM 视图矩阵
         * @param projectM 投影矩阵
         * @return ShaderProgram* 
         */
        static ShaderProgram *RenderUtil_GetShaderProgramWithBasicsData_Geometry(const glm::mat4 &viewM, const glm::mat4 &projectM) {
            auto sp = ShaderProgram::Get("Deferred-Geometry");
            sp->Use();
            sp->SetUniform(1, viewM);
            sp->SetUniform(2, projectM);
            return sp;
        }

        /**
         * @brief 执行渲染：灯光阶段
         * 
         * @param viewM 视图矩阵
         * @param projectM 透视矩阵
         */
        static void RenderLighting(const glm::mat4 &viewM, const glm::mat4 &projectM) {
            glDisable(GL_DEPTH_TEST);
            auto sp = ShaderProgram::Get("Deferred-Lighting");
            sp->Use();
            sp->SetUniform(0, viewM);
            sp->SetUniform(1, projectM);
            GBuffer().Use(sp);
            // Mesh::All_Instances
            glEnable(GL_DEPTH_TEST);
        }

        Material &getMaterial() {
            return Mat;
        }

    protected:
        Deferred(std::shared_ptr<Mesh> m, Material &&mat) : Base(std::move(m)), Mat(std::move(mat)) {
            shader_program_names.push_back("Deferred-Geometry");
            // shader_program_names.push_back("Deferred-Lighting");
        }

        Material Mat;
    };
}
