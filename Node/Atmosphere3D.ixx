/**
 * @file Atmosphere3D.ixx
 * @brief 氛围盒 / 天空背景
 * @version 1.0
 * @author Cheyne Xie
 * @date 2026/07/01
 *
 */

module;
#include <glm/glm.hpp>
#include <glad/glad.h>
export module CEngine.Node:Atmosphere3D;
import :Node3D;
import CEngine.Render;
import CEngine.Logger;
import std;

namespace CEngine {
    export class Atmosphere3D : public Node3D {
    public:
        NodeType GetType() override { return NodeType::Atmosphere3D; }
        bool IsType(NodeType type) override { return Node3D::IsType(type) || type == NodeType::Atmosphere3D; }

        static Atmosphere3D *Create() {
            return new Atmosphere3D();
        }

        /**
         * @brief 找到相机在 Radius 内距离最近的 Atmosphere3D 并渲染
         * 
         * @param atms Atmosphere3D 指针数组
         * @param viewM 视图矩阵
         * @param projectM 投影矩阵
         * @param camPos 相机位置
         */
        static Atmosphere3D* RenderAll(std::vector<Atmosphere3D *> &atms, const glm::mat4 &viewM, const glm::mat4 &projectM, const glm::vec3 &camPos) {
            auto distanceAndAtm = atms | std::views::transform([camPos](Atmosphere3D *atm) {
                return std::make_pair(glm::distance(atm->GetWorldPosition(), camPos), atm);
            });
            auto [distance, atm] = *std::ranges::min_element(distanceAndAtm);
            if (distance < atm->Radius){
                atm->RenderAtmosphere(viewM, projectM);
                return atm;
            }
            return nullptr;
        }

        /**
         * @brief 渲染 Atmosphere3D
         * 
         * @param viewM 视图矩阵
         * @param projectM 投影矩阵
         */
        virtual void RenderAtmosphere(const glm::mat4 &viewM, const glm::mat4 &projectM) {
            mIBL.SetActive();
            auto *sp = ShaderProgram::Get("Skybox");

            // 去掉平移，让天空跟随相机
            glm::mat4 viewNoTrans = glm::mat4(glm::mat3(viewM));
            sp->Use();
            sp->SetUniform("View", viewNoTrans);
            sp->SetUniform("Projection", projectM);

            // 环境立方图
            mIBL.EnsureGenerated();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, mIBL.getEnvCubemap());
            sp->SetUniform("environmentMap", 0);

            glDepthMask(GL_FALSE);
            glDepthFunc(GL_LEQUAL); // Skybox.vert 把 z 设为 w（NDC z=1.0），通过 LEQUAL 写入最远
            glDisable(GL_CULL_FACE); // 相机在立方体内部，需关闭剔除以渲染内表面
            if (auto cube = Mesh::GetEngineMesh("Cube")) cube->Render();
            glEnable(GL_CULL_FACE);
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);
        }

        void setRadius(float radius) { Radius = radius; }
        float getRadius() const { return Radius; }

        IBL& getIBL() { return mIBL; }

    protected:
        Atmosphere3D() = default;

        IBL mIBL;
        float Radius = 10;
    };
}
