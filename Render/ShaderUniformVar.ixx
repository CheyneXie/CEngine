/**
 * @file ShaderUniformVar.ixx
 * @brief ShaderUniformVar
 * @version 1.0
 * @author Chaim
 * @date 2024/10/05
 */

module;
#include <glm/glm.hpp>
export module CEngine.Render:ShaderUniformVar;
import :Texture;
import std;
import CEngine.Base;
import CEngine.Logger;

namespace CEngine {
    export class ShaderUniformVar {
    public:
        using shader_uniforms_var = std::variant<int, unsigned, float, double, glm::vec2, glm::vec3, glm::vec4, glm::mat3, glm::mat4, Texture *>;

        enum class Type : char {
            UNKNOWN,
            INT,
            UINT,
            FLOAT,
            DOUBLE,
            VEC2,
            VEC3,
            VEC4,
            MAT3,
            MAT4,
            SAMPLER2D
        };

        static Type StringToType(const std::string &str);
        static const char *TypeToString(const Type &type);

        ShaderUniformVar(const ShaderUniformVar &) = default;
        ShaderUniformVar &operator=(const ShaderUniformVar &) = default;
        ShaderUniformVar(ShaderUniformVar &&) = default;
        ShaderUniformVar &operator=(ShaderUniformVar &&) = default;

        template<typename T, typename=typename std::enable_if<!std::is_same_v<T, ShaderUniformVar>>::type>
        explicit ShaderUniformVar(const T &value) {
            if constexpr (std::is_same_v<T, ShaderUniformVar::Type>) {
                mType = value;
                // 默认值时 variant 类型必须与 mType 一致，否则 GetValue<T>() 会崩
                if (mType == Type::INT)                        Value = 0;
                else if (mType == Type::UINT)                  Value = 0u;
                else if (mType == Type::FLOAT)                 Value = 0.0f;
                else if (mType == Type::DOUBLE)                Value = 0.0;
                else if (mType == Type::VEC2)                  Value = glm::vec2(0);
                else if (mType == Type::VEC3)                  Value = glm::vec3(0);
                else if (mType == Type::VEC4)                  Value = glm::vec4(0);
                else if (mType == Type::MAT3)                  Value = glm::mat3(0);
                else if (mType == Type::MAT4)                  Value = glm::mat4(0);
                else if (mType == Type::SAMPLER2D)             Value = static_cast<Texture *>(nullptr);
            } else {
                if constexpr (std::is_same_v<T, int>) mType = Type::INT;
                else if constexpr (std::is_same_v<T, unsigned int>) mType = Type::UINT;
                else if constexpr (std::is_same_v<T, float>) mType = Type::FLOAT;
                else if constexpr (std::is_same_v<T, double>) mType = Type::DOUBLE;
                else if constexpr (std::is_same_v<T, glm::vec2>) mType = Type::VEC2;
                else if constexpr (std::is_same_v<T, glm::vec3>) mType = Type::VEC3;
                else if constexpr (std::is_same_v<T, glm::vec4>) mType = Type::VEC4;
                else if constexpr (std::is_same_v<T, glm::mat3>) mType = Type::MAT3;
                else if constexpr (std::is_same_v<T, glm::mat4>) mType = Type::MAT4;
                else if constexpr (std::is_same_v<T, Texture *>) mType = Type::SAMPLER2D;
                else static_assert(false, "ShaderUniformVars: 未适配类型");
                Value = value;
            }
        }

        Type GetType() const {
            return mType;
        }

        shader_uniforms_var &GetValue() {
            return Value;
        }

        template<typename T>
        T GetValue() {
            return std::get<T>(Value);
        }

        template<typename T>
        void SetValue(T value) {
            LogI("ShaderUniformVar") << "设置Shader Uniform : ShaderUniformVar(" << TypeToString(mType) << ") = " << value;
            Value = value;
        }

    private:
        Type mType;
        shader_uniforms_var Value;
    };

    ShaderUniformVar::Type ShaderUniformVar::StringToType(const std::string &str) {
        if (str == "int") return Type::INT;
        if (str == "uint") return Type::UINT;
        if (str == "float") return Type::FLOAT;
        if (str == "double") return Type::DOUBLE;
        if (str == "vec2") return Type::VEC2;
        if (str == "vec3") return Type::VEC3;
        if (str == "vec4") return Type::VEC4;
        if (str == "mat3") return Type::MAT3;
        if (str == "mat4") return Type::MAT4;
        if (str == "sampler2D") return Type::SAMPLER2D;
        LogE("ShaderUniformVar") << "存在未实现类型: " << str;
        return Type::UNKNOWN;
    }

    const char *ShaderUniformVar::TypeToString(const Type &type) {
        switch (type) {
            case Type::INT: return "int";
            case Type::UINT: return "uint";
            case Type::FLOAT: return "float";
            case Type::DOUBLE: return "double";
            case Type::VEC2: return "vec2";
            case Type::VEC3: return "vec3";
            case Type::VEC4: return "vec4";
            case Type::MAT3: return "mat3";
            case Type::MAT4: return "mat4";
            case Type::SAMPLER2D: return "sampler2D";
            default: return "unknown";
        }
    }

    // 为ShaderUniformVar::Type实现文本流
    export std::ostream &operator<<(std::ostream &os, const ShaderUniformVar::Type type) {
        os << ShaderUniformVar::TypeToString(type);
        return os;
    }
}

// 自定义哈希函数
namespace std {
    template<>
    struct hash<CEngine::ShaderUniformVar::Type> {
        size_t operator()(const CEngine::ShaderUniformVar::Type &type) const noexcept {
            return hash<char>()(static_cast<char>(type));
        }
    };

    template<>
    struct hash<std::pair<CEngine::ShaderUniformVar::Type, std::string> > {
        size_t operator()(const std::pair<CEngine::ShaderUniformVar::Type, std::string> &pair) const noexcept {
            return hash<string>()(pair.second) ^ hash<CEngine::ShaderUniformVar::Type>()(pair.first) << 1;
        }
    };
}
