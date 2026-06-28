/**
 * @file PBR.ixx
 * @author Cheyne Xie
 * @brief PBR(前向)渲染单元
 * @version 1.0
 * @date 2026-06-20
 * 
 */

module;
#include <glm/glm.hpp>
export module CEngine.RenderUnit:PBR;
import :Base;
import CEngine.Render;
import CEngine.Light;

namespace CEngine::RenderUnit {
    export class PBR final : public Base {
    public:
        Type GetType() override { return Type::PBR; }

        static std::unique_ptr<PBR> Create(std::shared_ptr<Mesh> m, Material &&mat) {
            return std::unique_ptr<PBR>(new PBR(std::move(m), std::move(mat)));
        }

        /**
         * @brief 批渲染
         * 
         * @param RUS 
         * @param viewM 视图矩阵
         * @param projectM 投影矩阵
         */
        static void RenderAll(std::vector<Base*>& RUS, const glm::mat4 &viewM, const glm::mat4 &projectM) {
            auto sp = RenderUtil_GetShaderProgramWithBasicsData(viewM, projectM);
            for (auto ru : RUS) {
                static_cast<PBR*>(ru)->Render(viewM, projectM, sp);
                Texture::ResetTextureSlot();
            }
        }

        /**
         * @brief 执行渲染
         * 
         * @param viewM 视图矩阵
         * @param projectM 投影矩阵
         * @param sp 用于批渲染时传入
         */
        void Render(const glm::mat4 &viewM, const glm::mat4 &projectM, ShaderProgram *sp = nullptr) {
            if (sp == nullptr)
                sp = RenderUtil_GetShaderProgramWithBasicsData(viewM, projectM);
            sp->SetUniform(0, WorldMatrix);
            Mat.Use(sp);
            RenderUtil_SetShaderUniform(sp);
            mesh->Render();
            RenderUtil_ResetShaderUniform(sp);
        }

        /**
         * @brief 工具函数：获取着色器程序并设置基础数据
         * 
         * @param viewM 视图矩阵
         * @param projectM 投影矩阵
         * @return ShaderProgram* 
         */
        static ShaderProgram *RenderUtil_GetShaderProgramWithBasicsData(const glm::mat4 &viewM, const glm::mat4 &projectM) {
            auto sp = ShaderProgram::Get("PBR");
            sp->Use();
            sp->SetUniform(1, viewM);
            sp->SetUniform(2, projectM);
            return sp;
        }

        Material &getMaterial() {
            return Mat;
        }

    protected:
        PBR(std::shared_ptr<Mesh> m, Material &&mat) : Base(std::move(m)), Mat(std::move(mat)) {
            shader_program_names.push_back("PBR");
        }

        Material Mat;
    };
}
