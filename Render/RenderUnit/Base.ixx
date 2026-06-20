/**
 * @file Base.ixx
 * @author Cheyne Xie
 * @brief 基础渲染单元
 * @version 1.0
 * @date 2026-06-20
 * 
 */

module;
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
export module CEngine.RenderUnit:Base;
import CEngine.Base;
import CEngine.Render;
import CEngine.Logger;
import std;

namespace CEngine {
    export class RenderUnit : public Object {
    public:
        static std::unique_ptr<RenderUnit> Create(Mesh *m) {
            return std::unique_ptr<RenderUnit>(new RenderUnit(m));
        }

        void RenderUtil_SetBasicsMartrix(ShaderProgram *sp, const glm::mat4 &worldM, const glm::mat4 &viewM, const glm::mat4 &projectM) {
            sp->SetUniform(0, worldM);
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
        void Render(const glm::mat4 &worldM, const glm::mat4 &viewM, const glm::mat4 &projectM) {
            auto sp = ShaderProgram::Get("Base");
            sp->Use();
            RenderUtil_SetBasicsMartrix(sp, worldM, viewM, projectM);
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

        RenderUnit(const RenderUnit&) = delete;
        RenderUnit& operator=(const RenderUnit&) = delete;
        RenderUnit(RenderUnit&&) = default;
        RenderUnit& operator=(RenderUnit&&) = default;

    protected:
        RenderUnit(Mesh *m) : mesh(m) {
            shader_program_names = { "Base" };
        }

        Mesh *mesh;
        std::vector<std::string_view> shader_program_names;
        std::unordered_map<std::string, ShaderUniformVar, StringHash, StringEqual> uniforms;
    };
}
