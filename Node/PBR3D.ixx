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
            RenderUnit3D::PreRender(viewM, projectM);
            shader_program->SetUniform("CameraPosition", camPos);
            // TODO Light
            Mat.Use(shader_program);
            RenderUnit3D::DoRender();
        }

        Material &getMaterial() {
            return Mat;
        }

    protected:
        PBR3D(Mesh *m, Material &&mat) : RenderUnit3D(m, "PBR"), Mat(std::move(mat)) {
        }

        Material Mat;
    };
}
