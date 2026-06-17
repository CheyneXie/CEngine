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
import CEngine.Node;
import CEngine.EventBus;
import CEngine.InputSystem;

namespace CEngine {
    export class FlyCamera3D final : public Behaviour {
    public:
        const static char *Name;

        FlyCamera3D(const char *name) : Behaviour(name) {
            RegisterParam<float>("Speed",
                [this]() { return this->speed * 1000; },
                [this](float v) { this->speed = abs(v * 0.001); }
            );
            RegisterParam<float>("Sensitivity",
                [this]() { return this->sensitivity; },
                [this](float v) { this->sensitivity = abs(v); }
            );
        }

        bool Ready() override {
            p3d = dynamic_cast<Camera3D *>(ParentNode);
            if (p3d == nullptr) {
                ParentNode->SetBehaviour(nullptr);
                return false;
            }
            EventBus().MouseScrollEvent += std::tuple(this, &FlyCamera3D::MouseScrollEvent);
            return true;
        }

        void MouseScrollEvent(void *window, double xpos, double ypos) {
            speed = abs(speed + ypos * 0.0000001);
        }

        void Update(const double DeltaTime) override {
            float delta = static_cast<float>(DeltaTime);

            if (InputSystem().Key(GLFW_KEY_W) == GLFW_PRESS) {
                p3d->SetPosition(p3d->GetPosition() - p3d->GetForward() * delta * speed);
            }
            if (InputSystem().Key(GLFW_KEY_S) == GLFW_PRESS) {
                p3d->SetPosition(p3d->GetPosition() + p3d->GetForward() * delta * speed);
            }

            if (InputSystem().Key(GLFW_KEY_A) == GLFW_PRESS) {
                p3d->SetPosition(p3d->GetPosition() - p3d->GetRight() * delta * speed);
            }
            if (InputSystem().Key(GLFW_KEY_D) == GLFW_PRESS) {
                p3d->SetPosition(p3d->GetPosition() + p3d->GetRight() * delta * speed);
            }

            if (InputSystem().Key(GLFW_KEY_E) == GLFW_PRESS) {
                p3d->SetPosition(p3d->GetPosition() + p3d->GetUp() * delta * speed);
            }
            if (InputSystem().Key(GLFW_KEY_Q) == GLFW_PRESS) {
                p3d->SetPosition(p3d->GetPosition() - p3d->GetUp() * delta * speed);
            }

            if (InputSystem().MouseButton(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
                auto cp = InputSystem().CursorPos();
                if (!(last_x == 0 && last_y == 0)) {
                    // const auto [w, h] = Engine::GetIns()->GetScreenSize();
                    const auto delta_x = static_cast<float>((cp.x - last_x) / sensitivity);
                    const auto delta_y = static_cast<float>((last_y - cp.y) / sensitivity);
                    const auto rotation = p3d->GetRotationPtr();
                    rotation->Yaw += delta_x;
                    rotation->Pitch = std::clamp(rotation->Pitch + delta_y, -glm::pi<float>() / 2.0f, glm::pi<float>() / 2.0f);
                    p3d->UpdateModelMatrix();
                }
                last_x = cp.x;
                last_y = cp.y;
            } else {
                last_x = last_y = 0;
            }
        }

    private:
        Camera3D *p3d = nullptr;
        // 设置变量
        float speed = 0.001f;
        float sensitivity = 1000.f;
        // 状态变量
        double last_x = 0, last_y = 0;
    };

    const char *FlyCamera3D::Name = "Fly Camera";
}
