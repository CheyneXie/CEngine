/**
 * @file ShaderProgram.ixx
 * @brief 着色器程序
 * @version 1.0
 * @author Chaim
 * @date 2024/10/05
 */

module;
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
export module CEngine.Render:ShaderProgram;
import :GLSL;
import :ShaderUniformVar;
import std;
import CEngine.Base;
import CEngine.Logger;
import CEngine.Utils;

namespace CEngine {
    export class ShaderProgram final : public Object {
    public:
        const static char *TAG;
        friend class ShaderManager;

        ShaderProgram(const ShaderProgram &) = delete;
        ShaderProgram &operator=(const ShaderProgram &) = delete;
        ShaderProgram(ShaderProgram &&) = delete;
        ShaderProgram &operator=(ShaderProgram &&) = delete;

        ~ShaderProgram() override {
            glDeleteProgram(shader_program_id);
        }

        /**
         * 创建ShaderProgram(自动生成Name)
         * @return 返回指针
         */
        static ShaderProgram *Create() {
            return new ShaderProgram();
        }

        /**
         * 创建ShaderProgram
         * @param name 名称(可通过All_Instances获取实例)
         * @return 返回指针
         */
        static ShaderProgram *Create(const std::string &name) {
            return new ShaderProgram(name);
        }

        /**
         * @brief 获取已创建的着色器程序
         * 
         * @param name 着色器程序名称
         * @return ShaderProgram* 着色器程序指针
         */
        static ShaderProgram *Get(std::string_view name) {
            auto sp = All_Instances.find(name);
            if (sp == All_Instances.end()) {
                LogE(TAG) << "未找到指定 ShaderProgram: " << name;
                return nullptr;
            }
            return sp->second;
        }

        /**
         * @brief 获取已创建着色器程序的数量
         * 
         * @return int 数量
         */
        static int Num() {
            return All_Instances.size();
        }

        /**
         * @brief Get All Instances
         * 
         * @return ShaderProgram::All_Instances
         */
        const static auto& Get() {
            return All_Instances;
        }

        /**
         * 添加Shader
         * @param shader GLSL对象
         * @return <code>self</code>可链式调用
         */
        ShaderProgram *AddShader(GLSL& shader) {
            glsl_list.push_back(shader.getName());
            std::unordered_set<std::pair<ShaderUniformVar::Type, std::string>> uls(UniformsList.begin(), UniformsList.end()); // 查重
            uls.insert(shader.getUniformsList().begin(), shader.getUniformsList().end());
            UniformsList.assign(uls.begin(), uls.end());
            glAttachShader(shader_program_id, shader.getShaderID());
            return this;
        }

        /**
         * 链接所有Shader
         */
        ShaderProgram *Link() {
            glLinkProgram(shader_program_id);
            int success_flag = 0;
            glGetProgramiv(shader_program_id, GL_LINK_STATUS, &success_flag);
            if (success_flag == GL_FALSE) {
                char info_log[512];
                glGetProgramInfoLog(shader_program_id, 512, nullptr, info_log);
                auto logger = LogE(TAG);
                logger << "着色器链接错误! " << Name << " (";
                for (const auto file: glsl_list) {
                    logger << file << ", ";
                }
                logger << "\b\b) \n";
                logger << "InfoLog: " << info_log;
                delete this;
                return nullptr;
            }
            auto logger = LogS(TAG);
            logger << "着色器链接成功: " << Name << " (";
            for (const auto file: glsl_list) {
                logger << file << ", ";
            }
            logger << "\b\b) ";
            All_Instances[Name] = this;
            return this;
        }

        void Use() const {
            glUseProgram(shader_program_id);
        }

        template<typename T>
        void SetUniform(const char *name, const T &value) {
            SetUniform(glGetUniformLocation(shader_program_id, name), value);
        }

        template<typename T>
        void SetUniform(const int location, const T &value) {
            if (location < 0) return;
            if constexpr (std::is_same_v<T, int>)
                glUniform1i(location, value);
            else if constexpr (std::is_same_v<T, int[]>)
                glUniform1iv(location, sizeof(value) / sizeof(T), &value[0]);
            else if constexpr (std::is_same_v<T, unsigned int>)
                glUniform1ui(location, value);
            else if constexpr (std::is_same_v<T, unsigned int[]>)
                glUniform1uiv(location, sizeof(value) / sizeof(T), &value[0]);
            else if constexpr (std::is_same_v<T, float>)
                glUniform1f(location, value);
            else if constexpr (std::is_same_v<T, float[]>)
                glUniform1fv(location, sizeof(value) / sizeof(T), &value[0]);
            else if constexpr (std::is_same_v<T, double>)
                glUniform1d(location, value);
            else if constexpr (std::is_same_v<T, double[]>)
                glUniform1dv(location, sizeof(value) / sizeof(T), &value[0]);
            else if constexpr (std::is_same_v<T, glm::vec2>)
                SetUniform(location, value.x, value.y);
            else if constexpr (std::is_same_v<T, glm::vec3>)
                SetUniform(location, value.x, value.y, value.z);
            else if constexpr (std::is_same_v<T, glm::vec4>)
                SetUniform(location, value.x, value.y, value.z, value.w);
            else if constexpr (std::is_same_v<T, glm::mat3>)
                glUniformMatrix3fv(location, 1, false, reinterpret_cast<const GLfloat *>(glm::value_ptr(value)));
            else if constexpr (std::is_same_v<T, glm::mat4>) {
                glUniformMatrix4fv(location, 1, false, reinterpret_cast<const GLfloat *>(glm::value_ptr(value)));
            } else static_assert(false, "SetUniform未实现该类型");
        }

        template<typename T>
        void SetUniform(const char *name, const T &value_x, const T &value_y) {
            SetUniform(glGetUniformLocation(shader_program_id, name), value_x, value_y);
        }

        template<typename T>
        void SetUniform(const int location, const T &value_x, const T &value_y) {
            if (location < 0) return;
            if constexpr (std::is_same_v<T, int>)
                glUniform2i(location, value_x, value_y);
            else if constexpr (std::is_same_v<T, unsigned int>)
                glUniform2ui(location, value_x, value_y);
            else if constexpr (std::is_same_v<T, float>)
                glUniform2f(location, value_x, value_y);
            else if constexpr (std::is_same_v<T, double>)
                glUniform2d(location, value_x, value_y);
            else static_assert(false, "SetUniform未实现该类型");
        }

        template<typename T>
        void SetUniform(const char *name, const T &value_x, const T &value_y, const T &value_z) {
            SetUniform(glGetUniformLocation(shader_program_id, name), value_x, value_y, value_z);
        }

        template<typename T>
        void SetUniform(const int location, const T &value_x, const T &value_y, const T &value_z) {
            if (location < 0) return;
            if constexpr (std::is_same_v<T, int>)
                glUniform3i(location, value_x, value_y, value_z);
            else if constexpr (std::is_same_v<T, unsigned int>)
                glUniform3ui(location, value_x, value_y, value_z);
            else if constexpr (std::is_same_v<T, float>)
                glUniform3f(location, value_x, value_y, value_z);
            else if constexpr (std::is_same_v<T, double>)
                glUniform3d(location, value_x, value_y, value_z);
            else static_assert(false, "SetUniform未实现该类型");
        }

        template<typename T>
        void SetUniform(const char *name, const T &value_x, const T &value_y, const T &value_z, const T &value_w) {
            SetUniform(glGetUniformLocation(shader_program_id, name), value_x, value_y, value_z, value_w);
        }

        template<typename T>
        void SetUniform(const int location, const T &value_x, const T &value_y, const T &value_z, const T &value_w) {
            if (location < 0) return;
            if constexpr (std::is_same_v<T, int>)
                glUniform4i(location, value_x, value_y, value_z, value_w);
            else if constexpr (std::is_same_v<T, unsigned int>)
                glUniform4ui(location, value_x, value_y, value_z, value_w);
            else if constexpr (std::is_same_v<T, float>)
                glUniform4f(location, value_x, value_y, value_z, value_w);
            else if constexpr (std::is_same_v<T, double>)
                glUniform4d(location, value_x, value_y, value_z, value_w);
            else static_assert(false, "SetUniform未实现该类型");
        }

        void SetShaderUniformVar(const char *name, ShaderUniformVar &uniform) {
            if (const auto type = uniform.GetType(); type == ShaderUniformVar::Type::INT)
                SetUniform(name, uniform.GetValue<int>());
            else if (type == ShaderUniformVar::Type::UINT)
                SetUniform(name, uniform.GetValue<unsigned int>());
            else if (type == ShaderUniformVar::Type::FLOAT)
                SetUniform(name, uniform.GetValue<float>());
            else if (type == ShaderUniformVar::Type::DOUBLE)
                SetUniform(name, uniform.GetValue<double>());
            else if (type == ShaderUniformVar::Type::VEC2)
                SetUniform(name, uniform.GetValue<glm::vec2>());
            else if (type == ShaderUniformVar::Type::VEC3)
                SetUniform(name, uniform.GetValue<glm::vec3>());
            else if (type == ShaderUniformVar::Type::VEC4)
                SetUniform(name, uniform.GetValue<glm::vec4>());
            else if (type == ShaderUniformVar::Type::MAT3)
                SetUniform(name, uniform.GetValue<glm::mat3>());
            else if (type == ShaderUniformVar::Type::MAT4)
                SetUniform(name, uniform.GetValue<glm::mat4>());
            else if (type == ShaderUniformVar::Type::SAMPLER2D) {
                if (const auto tex = uniform.GetValue<Texture *>(); tex != nullptr)
                    SetUniform(name, tex->Use());
            }
        }

        std::string getName() const { return Name; }

        /// @property shader_program_id
        unsigned int getShaderProgramID() const { return shader_program_id; }

        /// @property UniformsList
        std::vector<std::pair<ShaderUniformVar::Type, std::string> > &getUniformsList() { return UniformsList; }

    private:
        ShaderProgram() : ShaderProgram(Utils::GenerateUUID()) {
        }

        explicit ShaderProgram(const std::string &name) {
            shader_program_id = glCreateProgram();
            Name = name;
        }

        /// @brief 着色器程序ID
        unsigned int shader_program_id = 0;
        /// @brief 用于通过All_Instances调用的key
        std::string Name;
        /// @brief UniformsList
        std::vector<std::pair<ShaderUniformVar::Type, std::string>> UniformsList;
        /// @brief 该着色器程序所链接的GLSL，仅用于调试输出
        std::vector<std::string> glsl_list;

        static std::unordered_map<std::string, ShaderProgram *, StringHash, StringEqual> All_Instances;
    };

    const char *ShaderProgram::TAG = "ShaderProgram";
    std::unordered_map<std::string, ShaderProgram *, StringHash, StringEqual> ShaderProgram::All_Instances;
}
