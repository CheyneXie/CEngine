/**
* @file ModelImporter.ixx
 * @brief 模型导入器
 * @version 1.0
 * @author Chaim
 * @date 2024/10/06
 */

module;
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/ext/matrix_transform.hpp>
export module CEngine.ModelImporter;
import std;
import CEngine.Logger;
import CEngine.Render;
import CEngine.Node;
import CEngine.Utils;

namespace CEngine::ModelImporter {
    auto TAG = "ModelImporter";

    void process_node(const aiNode *node, const aiScene *scene, Node3D *parent, const char *model_path, std::string shader_program_name, const float transform_scale = 1.0f) {
        auto n3d = Node3D::Create();
        n3d->setName(node->mName.data);
        if (transform_scale == 1.0f)
            n3d->SetModelMatrix(Utils::aiMatrix4x4ToGlmMat4(node->mTransformation));
        else
            n3d->SetModelMatrix(glm::scale(Utils::aiMatrix4x4ToGlmMat4(node->mTransformation), glm::vec3(transform_scale)));
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            const aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            std::vector<VertexInfo> vertices;
            std::vector<unsigned int> indices;
            for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
                VertexInfo vertex;
                // 位置
                vertex.Position = {mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z};
                // 法线
                vertex.Normal = {mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z};
                // 切线
                glm::vec3 tangent = {mesh->mTangents[j].x, mesh->mTangents[j].y, mesh->mTangents[j].z};
                // 副切线
                glm::vec3 bitangent = {mesh->mBitangents[j].x, mesh->mBitangents[j].y, mesh->mBitangents[j].z};
                // 计算手性
                float handedness = glm::dot(glm::cross(vertex.Normal, tangent), bitangent) < 0.0f ? -1.0f : 1.0f;
                // 切线 + 手性
                vertex.Tangent = {tangent.x, tangent.y, tangent.z, handedness};
                // UV
                if (mesh->mTextureCoords[0])
                    vertex.TexCoord = {mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y};
                else
                    vertex.TexCoord = {0.0f, 0.0f};
                vertices.push_back(vertex);
            }
            for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
                const aiFace face = mesh->mFaces[j];
                for (unsigned int k = 0; k < face.mNumIndices; k++)
                    indices.push_back(face.mIndices[k]);
            }
            const auto m = Mesh::Create(vertices, indices);
            m->Name = mesh->mName.data;
            LogS(TAG) << "导入网格: " << m->Name;
            if (shader_program_name == "PBR") {
                const auto pbr3d = PBR3D::Create(m, Material::ProcessAssimpMaterial(scene->mMaterials[mesh->mMaterialIndex], model_path));
                n3d->AddChild(pbr3d);
            } else {
                const auto ru3d = RenderUnit3D::Create(m, shader_program_name);
                n3d->AddChild(ru3d);
            }
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            process_node(node->mChildren[i], scene, n3d, model_path, shader_program_name);
        }
        parent->AddChild(std::move(n3d));
    }

    export Node3D *import_model(const char *file_path, std::string shader_program_name = "Base", float transform_scale = 1.0f) {
        if (!Utils::FileExists(file_path)) {
            LogE(TAG) << "文件不存在: " << file_path;
            return nullptr;
        }
        LogI(TAG) << "开始导入模型: " << file_path;
        if (Utils::c_str_ends_with(file_path, ".fbx")) transform_scale = 0.1f;
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(file_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_CalcTangentSpace);
        if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            LogE(TAG) << importer.GetErrorString();
            return nullptr;
        }
        const auto node = Node3D::Create();
        node->setName(scene->mName.data);
        process_node(scene->mRootNode, scene, node, file_path, shader_program_name, transform_scale);
        return node;
    }
}
