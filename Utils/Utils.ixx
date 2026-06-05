//
// Created by chaim on 24-10-6.
//

module;
#include <assimp/matrix4x4.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string.h>
export module CEngine.Utils;
import std;
#ifdef _WIN32
import CEngine.Utils.WindowsFileDialog;
#endif

namespace CEngine::Utils {
    /**
     * @remark from: https://lowrey.me/guid-generation-in-c-11
     */
    unsigned char random_char() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);
        return static_cast<unsigned char>(dis(gen));
    }

    /**
     * 生成指定长度的随机十六进制字符串
     * @remark from: https://lowrey.me/guid-generation-in-c-11
     * @param len 长度
     * @return 字符串
     */
    export std::string generate_hex(const unsigned int len) {
        std::stringstream ss;
        for (auto i = 0; i < len; i++) {
            const auto rc = random_char();
            std::stringstream hexstream;
            hexstream << std::hex << static_cast<int>(rc);
            auto hex = hexstream.str();
            ss << (hex.length() < 2 ? '0' + hex : hex);
        }
        return ss.str();
    }

    export std::string GenerateUUID() {
        return generate_hex(32);
    }

    /**
     * assimp 4x4矩阵 转 glm 4x4矩阵
     * @param matrix aiMatrix4x4
     * @return glm::mat4
     */
    export glm::mat4 aiMatrix4x4ToGlmMat4(const aiMatrix4x4 &matrix) {
        return glm::mat4(matrix.a1, matrix.b1, matrix.c1, matrix.d1,
                         matrix.a2, matrix.b2, matrix.c2, matrix.d2,
                         matrix.a3, matrix.b3, matrix.c3, matrix.d3,
                         matrix.a4, matrix.b4, matrix.c4, matrix.d4);
    }


    /**
     * 判断一个字符串是否以指定的后缀结尾
     * @param str 指向要检查的字符串的指针
     * @param suffix 指向要检查的后缀的指针
     * @return 如果字符串以指定的后缀结尾，则返回<code>true</code>；如果字符串不是以指定的后缀结尾，或者任一参数为NULL，则返回<code>false</code>
     */
    export bool c_str_ends_with(const char *str, const char *suffix) {
        if (!str || !suffix) {
            return false;
        }
        const size_t str_len = strlen(str);
        const size_t suffix_len = strlen(suffix);
        if (suffix_len > str_len) {
            return false;
        }
        return strcmp(str + str_len - suffix_len, suffix) == 0;
    }

    /**
     * 十六进制转十六进制字符
     */
    export unsigned char HexToChar(const char c) {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        return 0; // 非法字符默认为0
    }

    export bool FileExists(const char *path) {
        return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
    }

    export bool DirectoryExists(const char *path) {
        return std::filesystem::exists(path) && std::filesystem::is_directory(path);
    }

    export std::string GetFileDir(const char *file_path) {
        std::filesystem::path p(file_path);
        std::filesystem::path parent = p.parent_path();
        if (parent.empty()) {
            return "";  // 没有目录部分（如 "file.txt"）
        }
        return parent.string().append(1, std::filesystem::path::preferred_separator); // 末尾加分隔符
    }

    export std::string GetFileName(const char *file_path, bool only_stem = false) {
        if (only_stem)
            return std::filesystem::path(file_path).stem().string();
        else
            return std::filesystem::path(file_path).filename().string();
    }

    export void vec3_to_float3(const glm::vec3 &in, float *out) {
        out[0] = in.x;
        out[1] = in.y;
        out[2] = in.z;
    }
}

// namespace CEngine::Utils {
//     export glm::vec3 Matrix4GetPosition(const glm::mat4 &matrix) {
//         return glm::vec3(matrix[3]);
//     }
//
//     export glm::vec3 Matrix4GetScale(const glm::mat4 &matrix) {
//         glm::vec3 scale;
//         scale.x = glm::length(glm::vec3(matrix[0]));
//         scale.y = glm::length(glm::vec3(matrix[1]));
//         scale.z = glm::length(glm::vec3(matrix[2]));
//         return scale;
//     }
//
//     export glm::quat Matrix4GetRotationQuaternion(const glm::mat4 &matrix) {
//         const glm::vec3 scale = Matrix4GetScale(matrix);
//         glm::mat3 rotationMatrix;
//         rotationMatrix[0] = glm::vec3(matrix[0]) / scale.x;
//         rotationMatrix[1] = glm::vec3(matrix[1]) / scale.y;
//         rotationMatrix[2] = glm::vec3(matrix[2]) / scale.z;
//         return glm::quat_cast(rotationMatrix);
//     }
//
//     export glm::vec3 Matrix4GetRotationEulerAngles(const glm::mat4 &matrix) {
//         return glm::eulerAngles(Matrix4GetRotationQuaternion(matrix));
//     }
//
//     export glm::vec3 Matrix4GetRotationAngles(const glm::mat4 &matrix) {
//         return glm::degrees(Matrix4GetRotationEulerAngles(matrix));
//     }
// }

namespace CEngine::Utils {
    /**
     * 打开文件对话框选择文件打开
     * @remark 取消返回空
     * @param file_filter 文件名过滤器，如<code>{ {L"Json", L"*.json"}, {L"TXT", L"*.txt"}}</code>
     * @return 返回文件路径
     */
    export std::filesystem::path ShowOpenFileDialog(const std::vector<std::pair<const wchar_t *, const wchar_t *> > &file_filter = {}) {
#ifdef _WIN32
        return WindowsFileDialog::OpenFile(file_filter);
#else
        return "";
#endif
    }

    /**
     * 打开文件对话框选择文件保存
     * @remark 取消返回空
     * @param file_filter 文件名过滤器，如<code>{ {L"Json", L"*.json"}, {L"TXT", L"*.txt"}}</code>
     * @return 返回文件路径
     */
    export std::filesystem::path ShowSaveFileDialog(const std::vector<std::pair<const wchar_t *, const wchar_t *> > &file_filter = {}) {
#ifdef _WIN32
        return WindowsFileDialog::SaveFile(file_filter);
#else
        return "";
#endif
    }

    /**
     * 选择文件夹
     * @remark 取消返回空
     * @return 文件夹路径
     */
    export std::filesystem::path ShowSelectFolderDialog() {
#ifdef _WIN32
        return WindowsFileDialog::SelectFolder();
#else
        return "";
#endif
    }
}
