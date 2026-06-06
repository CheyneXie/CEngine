/**
 * @file Rotator3D.ixx
 * @brief 飞行相机
 * @version 1.0
 * @author Chaim
 * @date 2024/10/21
 */

module;
#include <glm/glm.hpp>
#include <glm/ext/scalar_constants.hpp>
export module CEngine.Presets.Behaviours:Rotator3D;
import std;
import CEngine.Engine;
import CEngine.Node;

namespace CEngine {
    export class Rotator3D final : public Behaviour {
    public:
        const static char *Name;

        using Behaviour::Behaviour;

        bool Ready() override {
            p3d = dynamic_cast<Node3D *>(ParentNode);
            if (p3d == nullptr) {
                ParentNode->SetBehaviour(nullptr);
                return false;
            }
            return true;
        }

        void Update(const double DeltaTime) override {
            const auto rotation = p3d->GetRotationPtr();
            rotation->Yaw += DeltaTime / 500;
            p3d->UpdateModelMatrix();
        }

    private:
        Node3D *p3d = nullptr;
    };

    const char *Rotator3D::Name = "Model Rotator";
}
