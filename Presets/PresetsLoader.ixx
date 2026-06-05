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
import CEngine.Image;
import CEngine.Node;
import CEngine.Logger;
import CEngine.Utils;
import CEngine.Presets.Behaviours;

namespace CEngine {
    export class PresetsLoader {
    public:
        static const char *TAG;

        static void LoadAll() {
            ShaderManager::ShaderDirectory.push_back("Shader");
            LoadBehaviours();
            LoadTextures();
        }

        static void LoadBehaviours() {
            BehaviourFactory::Register<FlyCamera3D>();
        }

        static void LoadTextures() {
            auto data1 = new unsigned char[3]{ 255, 255, 255 };
            Texture::Create("<White>", {1, 1, data1, ColorMode::RGB}); // White
            delete data1;
            auto data2 = new unsigned char[3]{ 0, 0, 0 };
            Texture::Create("<Black>", {1, 1, data2, ColorMode::RGB}); // Black
            delete data2;
            auto data3 = new unsigned char[3]{ 127, 127, 127 };
            Texture::Create("<Gray>", {1, 1, data3, ColorMode::RGB}); // Gray
            delete data3;
            auto data4 = new unsigned char[3]{ 127, 127, 255 };
            Texture::Create("<DefalueNormal>", {1, 1, data4, ColorMode::RGB}); // Defalue Normal
            delete data4;
        }
    };

    const char *PresetsLoader::TAG = "预设加载器";
}
