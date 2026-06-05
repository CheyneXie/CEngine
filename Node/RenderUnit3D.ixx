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

        static RenderUnit3D *Create(Mesh *m, std::string shader_program_name) {
            return new RenderUnit3D(m, shader_program_name);
        }

        void PreRender(const glm::mat4 &viewM, const glm::mat4 &projectM) {
            shader_program->Use();
            shader_program->SetUniform(0, GetWorldMatrix());
            shader_program->SetUniform(1, viewM);
            shader_program->SetUniform(2, projectM);
        }

        void DoRender() {
            if (!uniforms.empty())
                for (auto [name, value]: uniforms) {
                    shader_program->SetShaderUniformVar(name.c_str(), value);
                }
            mesh->Render();
        }

        /**
         * 执行渲染
         */
        void Render(const glm::mat4 &viewM, const glm::mat4 &projectM) {
            PreRender(viewM, projectM);
            DoRender();
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
        ShaderProgram *getShaderProgram() const { return shader_program; }

        /// @property uniforms
        std::unordered_map<std::string, ShaderUniformVar> &getUniforms() { return uniforms; }

    protected:
        RenderUnit3D(Mesh *m, std::string shader_program_name) : mesh(m), shader_program_name(shader_program_name) {
            shader_program = ShaderProgram::All_Instances[shader_program_name];
            ShaderManager::Event_ReloadShader += [this]() { this->shader_program = ShaderProgram::All_Instances[this->shader_program_name]; };
        }

        Mesh *mesh;
        std::string shader_program_name;
        ShaderProgram *shader_program;
        std::unordered_map<std::string, ShaderUniformVar> uniforms;
    };
}
