/**
 * @file PBR3D.ixx
 * @brief 使用预设的PBRShader
 * @version 1.0
 * @author Chaim
 * @date 2024/10/05
 */

module;
#include <glm/glm.hpp>
export module CEngine.Node:PBR3D;
import :RenderUnit3D;
import CEngine.Render;

namespace CEngine {
    export class PBR3D final : public RenderUnit3D {
    public:
        const char *GetTypeName() override {
            return "PBR3D";
        }

        static PBR3D *Create(Mesh *m, Material &&mat) {
            return new PBR3D(m, std::move(mat));
        }

        void Render(const glm::mat4 &viewM, const glm::mat4 &projectM, const glm::vec3 &camPos) {
            auto sp = ShaderProgram::Get("PBR");
            sp->Use();
            RenderUtil_SetBasicsMartrix(sp, viewM, projectM);
            sp->SetUniform("CameraPosition", camPos);
            Mat.Use(sp);
            RenderUtil_SetShaderUniform(sp);
            mesh->Render();
        }

        Material &getMaterial() {
            return Mat;
        }

    protected:
        PBR3D(Mesh *m, Material &&mat) : RenderUnit3D(m), Mat(std::move(mat)) {
            shader_program_names = { "PBR" };
        }

        Material Mat;
    };
}
