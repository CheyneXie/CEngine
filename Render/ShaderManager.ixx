/**
 * @file ShaderManager.ixx
 * @brief ShaderManager
 * @version 1.0
 * @author Cheyne Xie
 * @date 2026/06/06
 */

export module CEngine.Render:ShaderManager;
import :GLSL;
import :ShaderProgram;
import CEngine.Utils;
import CEngine.EventBus;
import std;

namespace CEngine {
    export class ShaderManager {
    public:
        static const char *TAG;
        static std::vector<const char *> ShaderDirectory;

        static void LoadShaderProgram() {
            std::optional<std::vector<ShaderProgram *>> TempVec;
            if (ShaderProgram::Num() > 0) {
                TempVec = ShaderProgram::Get() | std::views::values | std::ranges::to<std::vector>();
                ShaderProgram::All_Instances.clear();
            }
            LogI(TAG) << "编译着色器...";
            for (auto directory : ShaderDirectory) {
                if (!Utils::DirectoryExists(directory)) {
                    LogE(TAG) << "着色器文件夹不存在: " << directory;
                    return;
                }
                for (const auto &file: std::filesystem::directory_iterator(directory)) {
                    auto vert_path = file.path().string();
                    if (!vert_path.ends_with("vert")) continue;
                    auto frag_path = std::string(vert_path);
                    frag_path.replace(frag_path.end() - 4, frag_path.end(), "frag");
                    if (!Utils::FileExists(frag_path.c_str())) continue;
                    auto _filename = file.path().filename().string();
                    const auto shader_name = _filename.substr(0, _filename.find_last_of('.'));
                    LogI(TAG) << "编译着色器: " << shader_name << " (" << vert_path << ", " << frag_path << ")";
                    auto vert = GLSL::FromFile(vert_path.c_str(), GLSL::ShaderType::Vertex);
                    auto frag = GLSL::FromFile(frag_path.c_str(), GLSL::ShaderType::Fragment);
                    if (!vert || !frag) {
                        LogE(TAG) << "着色器编译失败: " << shader_name << " (" << vert_path << ", " << frag_path << ")";
                    }
                    ShaderProgram::Create(shader_name)
                            ->AddShader(vert.get())
                            ->AddShader(frag.get())
                            ->Link();
                }
            }
            if (TempVec) {
                for (auto ptr : *TempVec)
                    delete ptr;
                EventBus().ShaderReloaded.Invoke();
            }
        }
    };
    std::vector<const char *> ShaderManager::ShaderDirectory;
    const char *ShaderManager::TAG = "Shader管理器";
}