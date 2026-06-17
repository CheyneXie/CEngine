/**
 * @file Camera3D.ixx
 * @brief 相机组件
 * @version 1.0
 * @author Chaim
 * @date 2024/10/19
 */

module;
#include <glm/ext/matrix_transform.hpp>
export module CEngine.Node:Camera3D;
import :Node3D;
import CEngine.Render;

namespace CEngine {
    export class Camera3D final : public Node3D, public Camera {
    public:
        static Camera3D *Create(const float fov = 75.0f, const float aspect_ratio = 16.0f / 9.0f, const float z_near = 0.1f, const float z_far = 100.0f) {
            return new Camera3D(fov, aspect_ratio, z_near, z_far);
        }

        const char *GetTypeName() override {
            return "Camera3D";
        }

        void UpdateViewMatrix() {
            ViewMatrix = Camera::GetViewMatrix(Position, -GetForward(true), GetUp(true));
        }

        Camera3D &SetPosition(const glm::vec3 &p, const bool updateM = true) override {
            Node3D::SetPosition(p, updateM);
            UpdateViewMatrix();
            return *this;
        }

        Camera3D &SetRotation(const EulerRotation &e, const bool updateM = true) override {
            Node3D::SetRotation(e, updateM);
            UpdateViewMatrix();
            return *this;
        }

        Camera3D &SetScale(const glm::vec3 &s, const bool updateM = true) override {
            Node3D::SetScale(s, updateM);
            UpdateViewMatrix();
            return *this;
        }

        Camera3D &UpdateModelMatrix() override {
            Node3D::UpdateModelMatrix();
            UpdateViewMatrix();
            return *this;
        }

        void SetModelMatrix(const glm::mat4 &matrix) override {
            Node3D::SetModelMatrix(matrix);
            UpdateViewMatrix();
        }

    private:
        explicit Camera3D(const float fov, const float aspect_ratio, const float z_near, const float z_far): Camera(fov, aspect_ratio, z_near, z_far) {
        }
    };
}
