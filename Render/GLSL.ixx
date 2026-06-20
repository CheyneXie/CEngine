/**
 * @file GLSL.ixx
 * @brief 读取GLSL文件并进行编译
 * @version 1.0
 * @author Chaim
 * @date 2024/10/05
 */

module;
#include <glad/glad.h>
#include <utility>
export module CEngine.Render:GLSL;
import :ShaderUniformVar;
import std;
import CEngine.Base;
import CEngine.Logger;
import CEngine.Utils;

namespace CEngine {
    const std::regex ShaderUniformPattern_1(R"(uniform\s(\w+?)\s(\w+))");

    /**
     * @brief GLSL文件类\n
     * 用于读取GLSL文件并进行编译\n
     * @remark 编译完成后请传入ShaderProgram类
     */
    export class GLSL final : public Object {
    public:
        /**
         * @brief 着色器类型枚举
         */
        enum class ShaderType {
            Vertex = GL_VERTEX_SHADER, /// 顶点着色器
            Fragment = GL_FRAGMENT_SHADER, /// 片元着色器
            Geometry = GL_GEOMETRY_SHADER, /// 几何着色器
            TessControl = GL_TESS_CONTROL_SHADER, /// 细分曲面控制着色器
            TessEvaluation = GL_TESS_EVALUATION_SHADER, /// 细分曲面验证着色器
            Compute = GL_COMPUTE_SHADER /// 计算着色器
        };

        static const char *TAG;

        /**
         * 从GLSL源码编译
         * @param glsl_source glsl源码
         * @param shader_type 着色器类型（枚举：ShaderType）
         * @param name 异常输出标识
         */
        static std::optional<GLSL> FromSource(const std::string &glsl_source, const ShaderType shader_type, const char *name = "源码编译") {
            const unsigned int id = glCreateShader(static_cast<GLenum>(shader_type));
            const char *source = glsl_source.c_str();
            glShaderSource(id, 1, &source, nullptr);
            glCompileShader(id);
            int success_flag = 0;
            glGetShaderiv(id, GL_COMPILE_STATUS, &success_flag);
            if (success_flag == GL_FALSE) {
                char info_log[512];
                glGetShaderInfoLog(id, 512, nullptr, info_log);
                LogE(TAG) << "着色器编译错误: " << name << "\nInfoLog: " << info_log;
                return std::nullopt;
            }
            return std::make_optional<GLSL>(id, name, GetShaderUniformsFromSource(glsl_source));
        }

        /**
         * 读取GLSL文件并编译
         * @param file_path 文件路径
         * @param shader_type 着色器类型（枚举：ShaderType）
         */
        static std::optional<GLSL> FromFile(const char *file_path, const ShaderType shader_type) {
            if (!Utils::FileExists(file_path)) {
                LogE(TAG) << "文件不存在: " << file_path;
                return std::nullopt;
            }
            LogI(TAG) << "正在编译着色器: " << file_path;
            std::ifstream file;
            file.exceptions(std::ifstream::badbit | std::ifstream::failbit);
            std::string glsl_source;
            try {
                file.open(file_path);
                std::stringstream ss;
                ss << file.rdbuf();
                glsl_source = ss.str();
                file.close();
            } catch (const std::ifstream::failure &e) {
                LogE(TAG) << "读取glsl文件时发生错误: " << file_path << "\n错误信息: " << e.what();
                return std::nullopt;
            }
            return FromSource(glsl_source, shader_type, file_path);
        }

        static std::unordered_set<std::pair<ShaderUniformVar::Type, std::string> > GetShaderUniformsFromSource(const std::string &glsl_source) {
            std::unordered_set<std::pair<ShaderUniformVar::Type, std::string> > uniforms;
            std::smatch match;
            auto it = glsl_source.cbegin();
            while (std::regex_search(it, glsl_source.cend(), match, ShaderUniformPattern_1)) {
                it = match[0].second;
                uniforms.insert({ShaderUniformVar::StringToType(match[1]), match[2]});
            }
            return std::move(uniforms);
        }

        GLSL(const unsigned int id, std::string name, std::unordered_set<std::pair<ShaderUniformVar::Type, std::string> > &&uniforms)
            : shader_id(id), UniformsList(std::move(uniforms)), Name(std::move(name)) {
        }

        GLSL(const GLSL &) = delete;
        GLSL &operator=(const GLSL &) = delete;
        GLSL(GLSL &&) = delete;
        GLSL &operator=(GLSL &&) = delete;

        ~GLSL() override {
            glDeleteShader(shader_id);
        }

        /// @property shader_id
        unsigned int getShaderID() const {
            return shader_id;
        }

        /// @property UniformsList
        std::unordered_set<std::pair<ShaderUniformVar::Type, std::string> > &getUniformsList() {
            return UniformsList;
        }

        /// @property Name
        std::string getName() const {
            return Name;
        }

    private:
        /// @brief 着色器ID
        unsigned int shader_id = 0;
        /// @brief GLSL中Uniform列表
        std::unordered_set<std::pair<ShaderUniformVar::Type, std::string> > UniformsList;
        /// 仅用作为异常输出标识
        std::string Name;
    };

    const char *GLSL::TAG = "GLSL";
}
