/**
 * @file Camera.ixx
 * @brief 相机
 * @version 1.0
 * @author Chaim
 * @date 2024/10/19
 */

module;
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
export module CEngine.Render:Camera;
import std;
import CEngine.Base;
import CEngine.EventBus;

namespace CEngine {
    export class Camera : public Object {
    public:
        explicit Camera(const float fov = 75.0f, const float aspect_ratio = 16.0f / 9.0f, const float z_near = 0.1f,
                        const float z_far = 5000.0f): FOV(fov), AspectRatio(aspect_ratio), zNear(z_near), zFar(z_far) {
            EventBus().CameraActivated += [this](void* c) {
                this->Activated = false;
            };
            UpdateProjectionMatrix();
        };

        void Activate() {
            EventBus().CameraActivated.Invoke(this);
            Activated = true;
        }
        bool IsActivated() const { return Activated; }

        float getFOV() const { return FOV; }
        void setFOV(const float fov) {
            FOV = fov;
            UpdateProjectionMatrix();
        }

        float getAspectRatio() const { return AspectRatio; }
        void setAspectRatio(const float aspect_ratio) {
            AspectRatio = aspect_ratio;
            UpdateProjectionMatrix();
        }

        std::pair<float, float> getClippingPlane() const { return {zNear, zFar}; }
        void setClippingPlane(const float z_near, const float z_far) {
            zNear = z_near;
            zFar = z_far;
            UpdateProjectionMatrix();
        }

        /// 获取透视矩阵
        virtual glm::mat4 GetProjectionMatrix() const { return ProjectionMatrix; }

        /// 需派生类实现
        virtual glm::mat4 GetViewMatrix() const { return ViewMatrix; }

        /// 获取View矩阵
        virtual glm::mat4 GetViewMatrix(const glm::vec3 &Position, const glm::vec3 &Front, const glm::vec3 &Up) const {
            return glm::lookAt(Position, Position + Front, Up);
        }

    protected:
        /// 更新透视矩阵
        void UpdateProjectionMatrix() {
            ProjectionMatrix = glm::perspective(glm::radians(FOV), AspectRatio, zNear, zFar);
        }

        /// 视角矩阵（该变量用于缓存）
        glm::mat4 ViewMatrix = glm::mat4(1.0f);
        /// 透视矩阵（该变量用于缓存）
        glm::mat4 ProjectionMatrix = glm::mat4(1.0f);
        /// 视场角
        float FOV;
        /// 长宽比
        float AspectRatio;
        /// 近裁剪平面
        float zNear;
        /// 远裁剪平面
        float zFar;

        bool Activated = false;
    };
}
