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
import CEngine.ModelImporter;

namespace CEngine {
    export class PresetsLoader {
    public:
        static const char *TAG;

        static void LoadAll() {
            LoadBehaviours();
            LoadTextures();
            LoadShader();
            LoadEngineMesh();
        }

        static void LoadBehaviours() {
            BehaviourFactory::Register<FlyCamera3D>();
            BehaviourFactory::Register<Rotator3D>();
        }

        static void LoadTextures() {
            auto data1 = new RGB[1]{ { 1.f, 1.f, 1.f } };
            Texture::Create("<White>", {1, 1, 8, data1 }); // White
            delete[] data1;
            auto data2 = new RGB[1]{ { 0.f, 0.f, 0.f } };
            Texture::Create("<Black>", {1, 1, 8, data2 }); // Black
            delete[] data2;
            auto data3 = new RGB[1]{ { 0.5f, 0.5f, 0.5f } };
            Texture::Create("<Gray>", {1, 1, 8, data3 }); // Gray
            delete[] data3;
            auto data4 = new RGB[1]{ { 0.5f, 0.5f, 1.f } };
            Texture::Create("<DefalueNormal>", {1, 1, 8, data4 }); // Defalue Normal
            delete[] data4;
        }

        static void LoadShader(bool _override = false) {
            std::string dirPath = Utils::GetExecutableDirectory() + "/Shader";
            std::error_code ec;
            std::filesystem::create_directories(dirPath, ec); 
            if (ec) {
                LogE(TAG) << "创建 Shader 文件夹失败: " << ec.message();
                return;
            }
            constexpr unsigned char Shader_Base_V[] = {
                #embed "CEngine/Presets/Shader/Base.vert"
            };
            constexpr unsigned char Shader_Base_F[] = {
                #embed "CEngine/Presets/Shader/Base.frag"
            };
            constexpr unsigned char Shader_PBR_V[] = {
                #embed "CEngine/Presets/Shader/PBR.vert"
            };
            constexpr unsigned char Shader_PBR_F[] = {
                #embed "CEngine/Presets/Shader/PBR.frag"
            };
            constexpr unsigned char Shader_Deferred_Geometry_V[] = {
                #embed "CEngine/Presets/Shader/Deferred-Geometry.vert"
            };
            constexpr unsigned char Shader_Deferred_Geometry_F[] = {
                #embed "CEngine/Presets/Shader/Deferred-Geometry.frag"
            };
            if(Utils::SaveFile(dirPath + "/Base.vert", Shader_Base_V, sizeof(Shader_Base_V), _override)
            && Utils::SaveFile(dirPath + "/Base.frag", Shader_Base_F, sizeof(Shader_Base_F), _override)
            && Utils::SaveFile(dirPath + "/PBR.vert", Shader_PBR_V, sizeof(Shader_PBR_V), _override)
            && Utils::SaveFile(dirPath + "/PBR.frag", Shader_PBR_F, sizeof(Shader_PBR_F), _override)
            && Utils::SaveFile(dirPath + "/Deferred-Geometry.vert", Shader_Deferred_Geometry_V, sizeof(Shader_Deferred_Geometry_V), _override)
            && Utils::SaveFile(dirPath + "/Deferred-Geometry.frag", Shader_Deferred_Geometry_F, sizeof(Shader_Deferred_Geometry_F), _override)) {
            } else {
                LogE(TAG) << "创建 GLSL 文件失败";
                return;
            }
            ShaderManager::ShaderDirectory.push_back("Shader");
            ShaderManager::LoadShaderProgram();
        }

        static void LoadEngineMesh() {
            constexpr unsigned char MeshData_FlatTriangle[] = {
                #embed "CEngine/Presets/Mesh/FlatTriangle.obj"
            };
            ModelImporter::ImportEngineMesh("FlatTriangle", MeshData_FlatTriangle, sizeof(MeshData_FlatTriangle));
            constexpr unsigned char MeshData_Panel[] = {
                #embed "CEngine/Presets/Mesh/Panel.obj"
            };
            ModelImporter::ImportEngineMesh("Panel", MeshData_Panel, sizeof(MeshData_Panel));
            constexpr unsigned char MeshData_Cube[] = {
                #embed "CEngine/Presets/Mesh/Cube.obj"
            };
            ModelImporter::ImportEngineMesh("Cube", MeshData_Cube, sizeof(MeshData_Cube));
            constexpr unsigned char MeshData_Sphere[] = {
                #embed "CEngine/Presets/Mesh/Sphere.obj"
            };
            ModelImporter::ImportEngineMesh("Sphere", MeshData_Sphere, sizeof(MeshData_Sphere));
            constexpr unsigned char MeshData_Sushan[] = {
                #embed "CEngine/Presets/Mesh/Sushan.obj"
            };
            ModelImporter::ImportEngineMesh("Sushan", MeshData_Sushan, sizeof(MeshData_Sushan));
        }
    };

    const char *PresetsLoader::TAG = "预设加载器";

    
}

