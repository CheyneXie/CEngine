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
import CEngine.RenderUnit;
import CEngine.Utils;

namespace CEngine::ModelImporter {
    auto TAG = "ModelImporter";

    /**
     * @brief 解析 assimp mesh
     * 
     * @param mesh assimp mesh指针
     * @param engine_mesh_name 如果是引擎模型，请提供名称
     * @return std::shared_ptr<Mesh> 
     */
    std::shared_ptr<Mesh> ProcessMesh(aiMesh *mesh, const char* engine_mesh_name = nullptr) {
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
        if (engine_mesh_name == nullptr) {
            auto m = Mesh::Create(vertices, indices);
            m->Name = std::string(mesh->mName.data);
            LogS(TAG) << "导入网格: " << mesh->mName.data;
            return std::move(m);
        } else {
            Mesh::CreateEngineMesh(engine_mesh_name, vertices, indices);
            LogS(TAG) << "导入网格: " << mesh->mName.data;
        }
        return nullptr;
    }

    /**
     * @brief 根据 assimp 场景创建 Node 树
     * 
     * @param node 当前 assimp 节点
     * @param scene assimp 场景
     * @param parent Node 树要添加到的父级
     * @param model_path 模型路径，用于查找纹理
     * @param render_type 渲染类型
     * @param transform_scale 模型缩放
     */
    void ProcessNode(const aiScene *scene, Node3D *parent, const char *model_path, RenderUnit::Type render_type, const float transform_scale = 1.0f) {
        std::stack<aiNode*> stack;
        stack.push(scene->mRootNode);
        while (!stack.empty()) {
            auto node = stack.top();
            stack.pop();
            stack.push_range(std::span(node->mChildren, node->mChildren + node->mNumChildren));

            auto n3d = Node3D::Create();
            n3d->setName(node->mName.data);
            n3d->SetModelMatrix(glm::scale(Utils::aiMatrix4x4ToGlmMat4(node->mTransformation), glm::vec3(transform_scale)));

            for (unsigned int i = 0; i < node->mNumMeshes; i++) {
                aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
                auto m = ProcessMesh(mesh);
                RenderUnit3D *ru3d = nullptr;
                switch (render_type) {
                    case RenderUnit::Type::Base: {
                        ru3d = RenderUnit3D::Create(RenderUnit::Base::Create(std::move(m)));
                        break;
                    }
                    case RenderUnit::Type::PBR: {
                        ru3d = RenderUnit3D::Create(RenderUnit::PBR::Create(std::move(m), Material::ProcessAssimpMaterial(scene->mMaterials[mesh->mMaterialIndex], model_path)));
                        break;
                    }
                    case RenderUnit::Type::Deferred_PBR: {
                        ru3d = RenderUnit3D::Create(RenderUnit::Deferred::Create(std::move(m), Material::ProcessAssimpMaterial(scene->mMaterials[mesh->mMaterialIndex], model_path)));
                        break;
                    }
                    default: {
                        LogE(TAG) << "渲染类型暂未实现";
                        break;
                    }
                }
                if (ru3d != nullptr)
                    n3d->AddChild(static_cast<Node*>(ru3d));
            }
            parent->AddChild(n3d);
        }
    }

    export Node3D *ImportModel(const char *file_path, RenderUnit::Type render_type = RenderUnit::Type::Base, float transform_scale = 1.0f) {
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
        ProcessNode(scene, node, file_path, render_type, transform_scale);
        return node;
    }

    export void ImportEngineMesh(const char* name, const void* buffer, size_t lenght) {
        LogI(TAG) << "开始导入引擎模型: " << name;
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFileFromMemory(buffer, lenght, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_CalcTangentSpace);
        if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            LogE(TAG) << importer.GetErrorString();
            return;
        }
        std::stack<aiNode*> stack;
        stack.push(scene->mRootNode);
        while (!stack.empty()) {
            auto node = stack.top();
            stack.pop();
            stack.push_range(std::span(node->mChildren, node->mChildren + node->mNumChildren));
            for (unsigned int i = 0; i < node->mNumMeshes; i++) {
                aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
                ProcessMesh(mesh, name);
            }
        }
    }
}
