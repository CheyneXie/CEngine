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
import :Type;
import CEngine.Base;
import CEngine.Render;
import CEngine.Logger;
import std;

namespace CEngine::RenderUnit {
    export class Base : public Object {
    public:
        virtual Type GetType() { return Type::Base; }

        static std::unique_ptr<Base> Create(std::shared_ptr<Mesh> m) {
            return std::unique_ptr<Base>(new Base(std::move(m)));
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
                ru->Render(viewM, projectM, sp);
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
            RenderUtil_SetShaderUniform(sp);
            mesh->Render();
        }

        /**
         * @brief 工具函数：获取着色器程序并设置基础数据
         * 
         * @param viewM 视图矩阵
         * @param projectM 投影矩阵
         * @return ShaderProgram* 
         */
        static ShaderProgram *RenderUtil_GetShaderProgramWithBasicsData(const glm::mat4 &viewM, const glm::mat4 &projectM) {
            auto sp = ShaderProgram::Get("Base");
            sp->Use();
            sp->SetUniform(1, viewM);
            sp->SetUniform(2, projectM);
            return sp;
        }

        /**
         * @brief 工具函数：设置 Uniform
         * 
         * @param sp 着色器程序
         */
        void RenderUtil_SetShaderUniform(ShaderProgram *sp) {
            if (!uniforms.empty())
                for (auto& [name, value]: uniforms) {
                    sp->SetShaderUniformVar(name.c_str(), value);
                }
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
        const std::shared_ptr<Mesh>& getMesh() const { return mesh; }

        /// @property shader_program
        const std::vector<std::string_view>& getShaderProgramNames() const { return shader_program_names; }

        /// @property uniforms
        std::unordered_map<std::string, ShaderUniformVar, StringHash, StringEqual> &getUniforms() { return uniforms; }

        Base(const Base&) = delete;
        Base& operator=(const Base&) = delete;
        Base(Base&&) = default;
        Base& operator=(Base&&) = default;

        glm::mat4 WorldMatrix;

    protected:
        Base(std::shared_ptr<Mesh> m) : mesh(std::move(m)) {
            shader_program_names = { "Base" };
        }

        std::shared_ptr<Mesh> mesh;
        std::vector<std::string_view> shader_program_names;
        std::unordered_map<std::string, ShaderUniformVar, StringHash, StringEqual> uniforms;
    };
}
