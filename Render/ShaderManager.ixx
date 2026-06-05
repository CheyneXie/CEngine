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
import CEngine.Event;
import CEngine.Utils;
import std;

namespace CEngine {
    export class ShaderManager {
    public:
        static const char *TAG;
        static std::vector<const char *> ShaderDirectory;
        static Event<void()> Event_ReloadShader;

        static void LoadShaderProgram() {
            bool is_reload = false;
            if (ShaderProgram::All_Instances.size() > 0) {
                is_reload = true;
                for (auto sp : (ShaderProgram::All_Instances | std::views::values))
                    delete sp;
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
            if (is_reload) Event_ReloadShader.Invoke();
        }
    };
    std::vector<const char *> ShaderManager::ShaderDirectory;
    Event<void()> ShaderManager::Event_ReloadShader;
    const char *ShaderManager::TAG = "Shader管理器";
}