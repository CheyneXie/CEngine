/**
 * @file Light3D.ixx
 * @brief 灯光
 * @version 1.0
 * @author Chaim
 * @date 2026/06/24
 */

module;
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
export module CEngine.Node:Light3D;
import :Node3D;
import CEngine.Light;
import std;

namespace CEngine {
    export class Light3D : public Node3D {
    public:
        NodeType GetType() override { return NodeType::Light3D; }
        bool IsType(NodeType type) override { return Node3D::IsType(type) || type == NodeType::Light3D; }

        static Light3D *Create(std::unique_ptr<Light::Base> ru) {
            return new Light3D(std::move(ru));
        }
        static Light3D *Create() {
            return Create(Light::Directional::Create());
        }

        Light::Base *GetLight() {
            return L.get();
        }

    protected: 
        Light3D(std::unique_ptr<Light::Base> ru) : L(std::move(ru)) {
            assert(L != nullptr && "Light3D 需要一个合法的 Light");
            auto type = L->GetType();
            if (type == Light::Type::Directional) {
                auto dl = static_cast<Light::Directional*>(L.get());
                if (!dl->SetDirectionManually) dl->setDirection(GetWorldRotation().RotateVector({0, -1, 0}));
                Event_ModelMatrixUpdated += [this](Node3D*) {
                    auto dl = static_cast<Light::Directional*>(this->L.get());
                    if (!dl->SetDirectionManually) dl->setDirection(this->GetWorldRotation().RotateVector({0, -1, 0}));
                };
            } else if (type == Light::Type::Point) {
                static_cast<Light::Point*>(L.get())->setPosition(GetWorldPosition());
                Event_ModelMatrixUpdated += [this](Node3D*) {
                    static_cast<Light::Point*>(this->L.get())->setPosition(GetWorldPosition());
                };
            } else {
                LogE("Light3D") << "暂未实现该灯光类型";
            }
        }

        std::unique_ptr<Light::Base> L;
    };
}
