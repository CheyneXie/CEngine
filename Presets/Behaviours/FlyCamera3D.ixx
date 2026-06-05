/**
 * @file FlyCamera3D.ixx
 * @brief 飞行相机
 * @version 1.0
 * @author Chaim
 * @date 2024/10/21
 */

module;
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/scalar_constants.hpp>
export module CEngine.Presets.Behaviours:FlyCamera3D;
import std;
import CEngine.Engine;
import CEngine.Node;

namespace CEngine {
    export class FlyCamera3D final : public Behaviour {
    public:
        const static char *Name;

        FlyCamera3D() = default;

        void Ready() override {
            p3d = dynamic_cast<Camera3D *>(ParentNode);
            if (p3d == nullptr) {
                ParentNode->SetBehaviour(nullptr);
                delete this;
                return;
            }
        }

        void Update(const double DeltaTime) override {
            const auto window = Engine::GetIns()->getWindow();
            const auto delta = static_cast<float>(DeltaTime);

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                p3d->SetPosition(p3d->GetPosition() - p3d->GetForward() * delta * speed);
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                p3d->SetPosition(p3d->GetPosition() + p3d->GetForward() * delta * speed);
            }

            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                p3d->SetPosition(p3d->GetPosition() - p3d->GetRight() * delta * speed);
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                p3d->SetPosition(p3d->GetPosition() + p3d->GetRight() * delta * speed);
            }

            if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
                p3d->SetPosition(p3d->GetPosition() + p3d->GetUp() * delta * speed);
            }
            if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
                p3d->SetPosition(p3d->GetPosition() - p3d->GetUp() * delta * speed);
            }

            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
                double x, y;
                glfwGetCursorPos(window, &x, &y);
                if (!(last_x == 0 && last_y == 0)) {
                    const auto [w, h] = Engine::GetIns()->GetScreenSize();
                    const auto delta_x = static_cast<float>((x - last_x) * sensitivity);
                    const auto delta_y = static_cast<float>((last_y - y) * sensitivity);
                    const auto rotation = p3d->GetRotationPtr();
                    rotation->Yaw += delta_x;
                    rotation->Pitch = std::clamp(rotation->Pitch + delta_y, -glm::pi<float>() / 2.0f, glm::pi<float>() / 2.0f);
                    p3d->UpdateModelMatrix();
                    // LogD("Camera") << rotation->Yaw << ", " << rotation->Pitch;
                    // LogD("Camera") << "正交检查: " << dot(p3d->GetForward(), p3d->GetRight()) << ", " << dot(p3d->GetForward(), p3d->GetUp()) << ", " << dot(p3d->GetRight(), p3d->GetUp());
                }
                last_x = x;
                last_y = y;
            } else {
                last_x = last_y = 0;
            }
        }

    private:
        Camera3D *p3d = nullptr;
        float speed = 0.01f;
        float sensitivity = 0.001f;
        double last_x = 0, last_y = 0;
    };

    const char *FlyCamera3D::Name = "漫游相机";
}
