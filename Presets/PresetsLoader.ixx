/**
 * @file PresetsLoader.ixx
 * @brief 预设加载器
 * @version 1.0
 * @author Chaim
 * @date 2024/10/21
 */

export module CEngine.PresetsLoader;
import std;
import CEngine.Base;
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
            BehaviourFactory::Register<Rotator3D>();
        }

        static void LoadTextures() {
            auto data1 = new Gray[1]{ 1.f };
            Texture::Create("<White>", {1, 1, 8, data1 }); // White
            delete[] data1;
            auto data2 = new Gray[1]{ 0.f };
            Texture::Create("<Black>", {1, 1, 8, data2 }); // Black
            delete[] data2;
            auto data3 = new Gray[1]{ 0.5f };
            Texture::Create("<Gray>", {1, 1, 8, data3 }); // Gray
            delete[] data3;
            auto data4 = new RGB[1]{ { 127.f, 127.f, 255.f } };
            Texture::Create("<DefalueNormal>", {1, 1, 8, data4 }); // Defalue Normal
            delete[] data4;
        }

        static void LoadEngineMesh() {
            
        }
    };

    const char *PresetsLoader::TAG = "预设加载器";
}
