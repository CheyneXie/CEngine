/**
 * @file RenderUnit3D.ixx
 * @brief 渲染单位
 * @version 1.0
 * @author Chaim
 * @date 2024/10/05
 */

module;
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
export module CEngine.Node:RenderUnit3D;
import :Node3D;
import std;
import CEngine.Render;

namespace CEngine {
    /**
     * @brief 渲染单位
     * @remark 网格+着色器+材质
     */
    export class RenderUnit3D : public Node3D {
    public:
        const char *GetTypeName() override {
            return "RenderUnit3D";
        }

        static RenderUnit3D *Create(Mesh *m) {
            return new RenderUnit3D(m);
        }

        void RenderUtil_SetBasicsMartrix(ShaderProgram *sp, const glm::mat4 &viewM, const glm::mat4 &projectM) {
            sp->SetUniform(0, GetWorldMatrix());
            sp->SetUniform(1, viewM);
            sp->SetUniform(2, projectM);
        }

        void RenderUtil_SetShaderUniform(ShaderProgram *sp) {
            if (!uniforms.empty())
                for (auto& [name, value]: uniforms) {
                    sp->SetShaderUniformVar(name.c_str(), value);
                }
        }

        /**
         * 执行渲染
         */
        void Render(const glm::mat4 &viewM, const glm::mat4 &projectM) {
            auto sp = ShaderProgram::Get("Base");
            sp->Use();
            RenderUtil_SetBasicsMartrix(sp, viewM, projectM);
            RenderUtil_SetShaderUniform(sp);
            mesh->Render();
        }

        /**
         * 设置着色器参数
         * @param name 变量名称
         * @param value 值
         */
        template<typename T>
        void SetShaderUniform(const std::string &name, T &&value) {
            LogI(TAG) << "设置Shader Uniform: " << name << " = " << value;
            if (uniforms.contains(name)) uniforms.erase(name);
            uniforms.emplace(name, ShaderUniformVar(value));
        }

        /// @property mesh
        Mesh *getMesh() const { return mesh; }

        /// @property shader_program
        const std::vector<std::string_view>& getShaderProgramNames() const { return shader_program_names; }

        /// @property uniforms
        std::unordered_map<std::string, ShaderUniformVar, StringHash, StringEqual> &getUniforms() { return uniforms; }

    protected:
        RenderUnit3D(Mesh *m) : mesh(m) {
            shader_program_names = { "Base" };
        }

        Mesh *mesh;
        std::vector<std::string_view> shader_program_names;
        std::unordered_map<std::string, ShaderUniformVar, StringHash, StringEqual> uniforms;
    };
}
