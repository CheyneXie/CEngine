/**
 * @file PresetsLoader.ixx
 * @brief 预设加载器
 * @version 1.0
 * @author Chaim
 * @date 2024/10/21
 */

export module CEngine.PresetsLoader;
import std;
import CEngine.Render;
import CEngine.Node;
import CEngine.Logger;
import CEngine.Utils;
import CEngine.Presets.Behaviours;

namespace CEngine {
    export class PresetsLoader {
    public:
        static const char *TAG;

        static void LoadAll() {
            LoadShaderProgram();
            LoadBehaviours();
        }

        static void LoadShaderProgram() {
            const auto presets_shader_directory = "Shader";
            LogI(TAG) << "加载预设着色器...";
            if (!std::filesystem::exists(presets_shader_directory) || !std::filesystem::is_directory(presets_shader_directory)) {
                LogE(TAG) << "预设着色器文件夹不存在: " << presets_shader_directory;
                return;
            }
            for (const auto &file: std::filesystem::directory_iterator(presets_shader_directory)) {
                auto vert_path = file.path().string();
                if (!vert_path.ends_with("vert")) continue;
                auto frag_path = std::string(vert_path);
                frag_path.replace(frag_path.end() - 4, frag_path.end(), "frag");
                if (!Utils::FileExists(frag_path.c_str())) continue;
                auto _filename = file.path().filename().string();
                const auto shader_name = _filename.substr(0, _filename.find_last_of('.'));
                LogI(TAG) << "加载预设着色器: " << shader_name << " (" << vert_path << ", " << frag_path << ")";
                auto vert = GLSL::FromFile(vert_path.c_str(), GLSL::ShaderType::Vertex);
                auto frag = GLSL::FromFile(frag_path.c_str(), GLSL::ShaderType::Fragment);
                if (!vert || !frag) {
                    LogE(TAG) << "预设着色器加载失败: " << shader_name << " (" << vert_path << ", " << frag_path << ")";
                }
                ShaderProgram::Create(shader_name)
                        ->AddShader(vert.get())
                        ->AddShader(frag.get())
                        ->Link();
            }
        }

        static void LoadBehaviours() {
            BehaviourFactory::Register<FlyCamera3D>();
        }
    };

    const char *PresetsLoader::TAG = "预设加载器";



}
