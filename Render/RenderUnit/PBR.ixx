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

namespace CEngine {
    export class PBR final : public RenderUnit {
    public:
        static std::unique_ptr<PBR> Create(Mesh *m, Material &&mat) {
            return std::unique_ptr<PBR>(new PBR(m, std::move(mat)));
        }

        void Render(const glm::mat4 &worldM, const glm::mat4 &viewM, const glm::mat4 &projectM, const glm::vec3 &camPos) {
            auto sp = ShaderProgram::Get("PBR");
            sp->Use();
            RenderUtil_SetBasicsMartrix(sp, worldM, viewM, projectM);
            sp->SetUniform("CameraPosition", camPos);
            Mat.Use(sp);
            RenderUtil_SetShaderUniform(sp);
            mesh->Render();
        }

        Material &getMaterial() {
            return Mat;
        }

    protected:
        PBR(Mesh *m, Material &&mat) : RenderUnit(m), Mat(std::move(mat)) {
            shader_program_names = { "PBR" };
        }

        Material Mat;
    };
}
