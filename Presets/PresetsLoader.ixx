/**
 * @file PresetsLoader.ixx
 * @brief 预设加载器
 * @version 1.0
 * @author Chaim
 * @date 2024/10/21
 */

module;
#include "Presets/Shader/Base.vert.h"
#include "Presets/Shader/Base.frag.h"
#include "Presets/Shader/PBR.vert.h"
#include "Presets/Shader/PBR.frag.h"
#include "Presets/Shader/Deferred-Geometry.vert.h"
#include "Presets/Shader/Deferred-Geometry.frag.h"
#include "Presets/Shader/Deferred-Lighting.vert.h"
#include "Presets/Shader/Deferred-Lighting.frag.h"
#include "Presets/Shader/IBL_EquirectToCube.vert.h"
#include "Presets/Shader/IBL_EquirectToCube.frag.h"
#include "Presets/Shader/IBL_Irradiance.vert.h"
#include "Presets/Shader/IBL_Irradiance.frag.h"
#include "Presets/Shader/IBL_Prefilter.vert.h"
#include "Presets/Shader/IBL_Prefilter.frag.h"
#include "Presets/Shader/IBL_BRDFLUT.vert.h"
#include "Presets/Shader/IBL_BRDFLUT.frag.h"
#include "Presets/Shader/Skybox.vert.h"
#include "Presets/Shader/Skybox.frag.h"
#include "Presets/Shader/Postprocess.vert.h"
#include "Presets/Shader/Postprocess.frag.h"

#include "Presets/Mesh/Cube.obj.h"
#include "Presets/Mesh/FlatTriangle.obj.h"
#include "Presets/Mesh/Panel.obj.h"
#include "Presets/Mesh/Sphere.obj.h"
#include "Presets/Mesh/Sushan.obj.h"

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
            #ifdef NDEBUG
            LoadShader();
            #else
            LoadShader(true);
            #endif
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
            const bool ok =
                Utils::SaveFile(dirPath + "/Base.vert", Base_vert, Base_vert_len, _override)
                && Utils::SaveFile(dirPath + "/Base.frag", Base_frag, Base_frag_len, _override)
                && Utils::SaveFile(dirPath + "/PBR.vert", PBR_vert, PBR_vert_len, _override)
                && Utils::SaveFile(dirPath + "/PBR.frag", PBR_frag, PBR_frag_len, _override)
                && Utils::SaveFile(dirPath + "/Deferred-Geometry.vert", Deferred_Geometry_vert, Deferred_Geometry_vert_len, _override)
                && Utils::SaveFile(dirPath + "/Deferred-Geometry.frag", Deferred_Geometry_frag, Deferred_Geometry_frag_len, _override)
                && Utils::SaveFile(dirPath + "/Deferred-Lighting.vert", Deferred_Lighting_vert, Deferred_Lighting_vert_len, _override)
                && Utils::SaveFile(dirPath + "/Deferred-Lighting.frag", Deferred_Lighting_frag, Deferred_Lighting_frag_len, _override)
                && Utils::SaveFile(dirPath + "/IBL_EquirectToCube.vert", IBL_EquirectToCube_vert, IBL_EquirectToCube_vert_len, _override)
                && Utils::SaveFile(dirPath + "/IBL_EquirectToCube.frag", IBL_EquirectToCube_frag, IBL_EquirectToCube_frag_len, _override)
                && Utils::SaveFile(dirPath + "/IBL_Irradiance.vert", IBL_Irradiance_vert, IBL_Irradiance_vert_len, _override)
                && Utils::SaveFile(dirPath + "/IBL_Irradiance.frag", IBL_Irradiance_frag, IBL_Irradiance_frag_len, _override)
                && Utils::SaveFile(dirPath + "/IBL_Prefilter.vert", IBL_Prefilter_vert, IBL_Prefilter_vert_len, _override)
                && Utils::SaveFile(dirPath + "/IBL_Prefilter.frag", IBL_Prefilter_frag, IBL_Prefilter_frag_len, _override)
                && Utils::SaveFile(dirPath + "/IBL_BRDFLUT.vert", IBL_BRDFLUT_vert, IBL_BRDFLUT_vert_len, _override)
                && Utils::SaveFile(dirPath + "/IBL_BRDFLUT.frag", IBL_BRDFLUT_frag, IBL_BRDFLUT_frag_len, _override)
                && Utils::SaveFile(dirPath + "/Skybox.vert", Skybox_vert, Skybox_vert_len, _override)
                && Utils::SaveFile(dirPath + "/Skybox.frag", Skybox_frag, Skybox_frag_len, _override)
                && Utils::SaveFile(dirPath + "/Postprocess.vert", Postprocess_vert, Postprocess_vert_len, _override)
                && Utils::SaveFile(dirPath + "/Postprocess.frag", Postprocess_frag, Postprocess_frag_len, _override);
            if (!ok) {
                LogE(TAG) << "创建 GLSL 文件失败";
                return;
            }
            static std::string shaderDir = dirPath; // static 字符串保活
            ShaderManager::ShaderDirectory.push_back(shaderDir.c_str());
            ShaderManager::LoadShaderProgram();
        }

        static void LoadEngineMesh() {
            ModelImporter::ImportEngineMesh("FlatTriangle", FlatTriangle_obj, FlatTriangle_obj_len);
            
            ModelImporter::ImportEngineMesh("Panel", Panel_obj, Panel_obj_len);
            
            ModelImporter::ImportEngineMesh("Cube", Cube_obj, Cube_obj_len);
            
            ModelImporter::ImportEngineMesh("Sphere", Sphere_obj, Sphere_obj_len);
            
            ModelImporter::ImportEngineMesh("Sushan", Sushan_obj, Sushan_obj_len);
        }
    };

    const char *PresetsLoader::TAG = "预设加载器";

    
}

