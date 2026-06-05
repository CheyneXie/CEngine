/**
 * @file Material.ixx
 * @brief 材质(PBR)相关
 * @version 1.0
 * @author Chaim
 * @date 2024/10/06
 */

module;
#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
export module CEngine.Render:Material;
import :Texture;
import :ShaderProgram;
import CEngine.Utils;

namespace CEngine {
    export class Material {
    public:

        Material() {
            glGenBuffers(1, &UBO_Parameters);
        }

        static Material ProcessAssimpMaterial(const aiMaterial *material, const char *model_file_path) {
            auto file_dir = Utils::GetFileDir(model_file_path);
            Material mat;
            // 读取、上传贴图
            for (auto &[key, value]: mat.Textures) {
                if (!value.second) continue;
                if (material->GetTextureCount(key) > 0) {
                    aiString tex_path;
                    material->GetTexture(key, 0, &tex_path);
                    if (tex_path.length > 0)
                        value.first = Texture::FromFile(std::format("{}{}", file_dir, tex_path.data).c_str());
                }
            }
            // 读取参数
            material->Get(AI_MATKEY_EMISSIVE_INTENSITY, mat.Parameters.EMISSIVE_INTENSITY);
            // if (mat.Textures[aiTextureType_METALNESS].first == nullptr)
                material->Get(AI_MATKEY_METALLIC_FACTOR, mat.Parameters.METALLIC);
            // if (mat.Textures[aiTextureType_DIFFUSE_ROUGHNESS].first == nullptr)
                material->Get(AI_MATKEY_ROUGHNESS_FACTOR, mat.Parameters.ROUGHNESS);
            material->Get(AI_MATKEY_OPACITY, mat.Parameters.OPACITY);
            // if (mat.Textures[aiTextureType_DIFFUSE].first == nullptr)
                material->Get(AI_MATKEY_COLOR_DIFFUSE, mat.Parameters.DIFFUSE_COLOR);
            // if (mat.Textures[aiTextureType_SPECULAR].first == nullptr)
            //     material->Get(AI_MATKEY_COLOR_SPECULAR, mat.Parameters.SPECULAR_COLOR);
            // if (mat.Textures[aiTextureType_EMISSION_COLOR].first == nullptr)
                material->Get(AI_MATKEY_COLOR_EMISSIVE, mat.Parameters.EMISSION_COLOR);
            // if (mat.Textures[aiTextureType_REFLECTION].first == nullptr)
                // material->Get(AI_MATKEY_COLOR_REFLECTIVE, mat.Parameters.REFLECTIVE_COLOR);
            // material->Get(AI_MATKEY_COLOR_TRANSPARENT, mat.Parameters.TRANSPARENT_COLOR);
            mat.UpdateParameters();
            return std::move(mat);
        }

        /**
        * { Assimp纹理类型枚举, {Texture指针, 是否启用} }
        */
        // @formatter:off
        std::unordered_map<aiTextureType, std::pair<Texture *, bool> > Textures = {
            {aiTextureType_DIFFUSE,             {nullptr, true}},   // 漫反射纹理
            // {aiTextureType_SPECULAR,            {nullptr, true}},   // 镜面反射纹理
            // {aiTextureType_AMBIENT,             {nullptr, false}},  // 环境光纹理
            {aiTextureType_EMISSIVE,            {nullptr, true}},   // 自发光纹理
            {aiTextureType_HEIGHT,              {nullptr, true}},   // 高度纹理
            {aiTextureType_NORMALS,             {nullptr, true}},   // 法线纹理
            // {aiTextureType_SHININESS,           {nullptr, false}},  // 光泽度纹理
            {aiTextureType_OPACITY,             {nullptr, true}},   // 不透明度纹理
            {aiTextureType_DISPLACEMENT,        {nullptr, true}},   // 位移纹理
            // {aiTextureType_LIGHTMAP,            {nullptr, true}},   // 光照贴图纹理
            // {aiTextureType_REFLECTION,          {nullptr, true}},   // 反射纹理
            {aiTextureType_BASE_COLOR,          {nullptr, true}},   // 基础颜色纹理
            // {aiTextureType_NORMAL_CAMERA,       {nullptr, false}},  // 摄像机空间法线纹理
            {aiTextureType_EMISSION_COLOR,      {nullptr, true}},   // 发射颜色纹理
            {aiTextureType_METALNESS,           {nullptr, true}},   // 金属度纹理
            {aiTextureType_DIFFUSE_ROUGHNESS,   {nullptr, true}},   // 漫反射粗糙度纹理
            {aiTextureType_AMBIENT_OCCLUSION,   {nullptr, true}},   // 环境遮蔽纹理
            // {aiTextureType_SHEEN,               {nullptr, false}},  // 光泽纹理
            // {aiTextureType_CLEARCOAT,           {nullptr, false}},  // 清漆纹理
            // {aiTextureType_TRANSMISSION,        {nullptr, false}}   // 透射纹理
        };

        /// @remark 请据此顺序在GLSL中构建结构体
        struct MParameters {
            float EMISSIVE_INTENSITY    = 1.0f; // 发射强度
            float METALLIC              = 1.0f; // 金属度
            float ROUGHNESS             = 1.0f; // 粗糙度
            float OPACITY               = 1.0f; // 透明度
            aiColor4D DIFFUSE_COLOR     = {1.0f, 1.0f, 1.0f, 1.0f}; // 漫反射颜色
            // aiColor4D SPECULAR_COLOR    = {1.0f, 1.0f, 1.0f, 1.0f}; // 镜面反射颜色
            aiColor4D EMISSION_COLOR    = {1.0f, 1.0f, 1.0f, 1.0f}; // 发射颜色
            // aiColor4D REFLECTIVE_COLOR  = {1.0f, 1.0f, 1.0f, 1.0f}; // 反射颜色
            // aiColor4D TRANSPARENT_COLOR = {1.0f, 1.0f, 1.0f, 1.0f}; // 透明度（无对应纹理）（不知道是啥）
        };
        // @formatter:on

        MParameters Parameters;

        /**
         * 触发上传参数
         */
        void UpdateParameters() {
            glBindBuffer(GL_UNIFORM_BUFFER, UBO_Parameters);
            glBufferData(GL_UNIFORM_BUFFER, sizeof(MParameters), &Parameters, GL_STATIC_DRAW);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        /**
         * 使用材质
         * @remark 请先<code>Use</code>ShaderProgram
         * @remark 请保证GLSL中材质参数结构体与MParameters结构相同
         * @remark 请保证在第一个材质贴图
         * @param shader \n
         * ShaderProgram指针
         * @param material_parameters_uniform_block_index \n
         * 材质参数Uniform结构索引 (使用函数<code>glGetUniformBlockIndex()</code>) \n
         * 若不传参，则自动搜索结构<code>Material_Parameters</code>
         */
        void Use(const ShaderProgram *shader, const int material_parameters_uniform_block_index = -1) const {
            const auto shader_id = shader->getShaderProgramID();
            const unsigned int i = material_parameters_uniform_block_index < 0
                                       ? glGetUniformBlockIndex(shader_id, "Material_Parameters")
                                       : material_parameters_uniform_block_index;
            // TODO 避免固定BINDING_POINT
            constexpr unsigned int BINDING_POINT = 0;
            glUniformBlockBinding(shader_id, i, BINDING_POINT);
            glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_POINT, UBO_Parameters);

            /* 寻址法 */
            bool exists[] = {false, false, false, false, false, false, false, false};
            for (auto &[type, value]: Textures) {
                // 纹理不存在或不启用
                if (value.first == nullptr || !value.second) continue;
                // 纹理存在
                const char *name = nullptr;
                // @formatter:off
                switch (type) {
                    case aiTextureType_DIFFUSE:             name = "Tex_BaseColor";         exists[0] = true; break;
                    case aiTextureType_EMISSIVE:            name = "Tex_Emissive";          exists[1] = true; break;
                    case aiTextureType_HEIGHT:              name = "Tex_Height";            exists[2] = true; break;
                    case aiTextureType_NORMALS:             name = "Tex_Normals";           exists[3] = true; break;
                    case aiTextureType_OPACITY:             name = "Tex_Opacity";           exists[4] = true; break;
                    case aiTextureType_DISPLACEMENT:        name = "Tex_Height";            exists[2] = true; break;
                    case aiTextureType_BASE_COLOR:          name = "Tex_BaseColor";         exists[0] = true; break;
                    case aiTextureType_EMISSION_COLOR:      name = "Tex_Emissive";          exists[1] = true; break;
                    case aiTextureType_METALNESS:           name = "Tex_Metalness";         exists[5] = true; break;
                    case aiTextureType_DIFFUSE_ROUGHNESS:   name = "Tex_Roughness";         exists[6] = true; break;
                    case aiTextureType_AMBIENT_OCCLUSION:   name = "Tex_AmbientOcclusion";  exists[7] = true; break;
                    default: continue;
                }
                // @formatter:on
                set_texture_uniform(shader_id, name, value.first);
            }
            // 设置默认纹理
            for (int i = 0; i < 8; i++) {
                if (!exists[i])
                    switch (i) {
                        case 0: set_texture_uniform(shader_id, "Tex_BaseColor", Texture::All_Instances["<White>"]); break;
                        case 1: set_texture_uniform(shader_id, "Tex_Emissive", Texture::All_Instances["<Black>"]); break;
                        case 2: set_texture_uniform(shader_id, "Tex_Height", Texture::All_Instances["<Black>"]); break;
                        case 3: set_texture_uniform(shader_id, "Tex_Normals", Texture::All_Instances["<DefalueNormal>"]); break;
                        case 4: set_texture_uniform(shader_id, "Tex_Opacity", Texture::All_Instances["<White>"]); break;
                        case 5: set_texture_uniform(shader_id, "Tex_Metalness", Texture::All_Instances["<Gray>"]); break;
                        case 6: set_texture_uniform(shader_id, "Tex_Roughness", Texture::All_Instances["<Gray>"]); break;
                        case 7: set_texture_uniform(shader_id, "Tex_AmbientOcclusion", Texture::All_Instances["<White>"]); break;
                        default: break;
                    }
            }
        }

    private:
        // 工具函数
        void set_texture_uniform(unsigned int shader_id, const char *name, Texture *texture) const {
            if(const auto location = glGetUniformLocation(shader_id, name); location >= 0) {
                glUniform1i(location, texture->Use());
            }
        }

        unsigned int UBO_Parameters = 0;
    };
}
