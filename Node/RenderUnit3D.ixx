/**
 * @file RenderUnit3D.ixx
 * @brief 渲染单位
 * @version 1.0
 * @author Chaim
 * @date 2024/10/05
 */

module;
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
export module CEngine.Node:RenderUnit3D;
import :Node3D;
import CEngine.RenderUnit;
import std;

namespace CEngine {
    /**
     * @brief 渲染单位
     * @remark 网格+着色器+材质
     */
    export class RenderUnit3D : public Node3D {
    public:
        NodeType GetType() override { return NodeType::RenderUnit3D; }
        bool IsType(NodeType type) override { return Node3D::IsType(type) || type == NodeType::RenderUnit3D; }

        static RenderUnit3D *Create(std::unique_ptr<RenderUnit::Base> ru) {
            return new RenderUnit3D(std::move(ru));
        }

        RenderUnit::Base *GetRU() {
            return RU.get();
        }

    protected:
        RenderUnit3D(std::unique_ptr<RenderUnit::Base> ru) : RU(std::move(ru)) {
            assert(RU != nullptr && "RenderUnit3D 需要一个合法的 RenderUnit");
            RU->WorldMatrix = GetWorldMatrix();
            Event_ModelMatrixUpdated += [this](Node3D*) {
                this->RU->WorldMatrix = this->GetWorldMatrix();
            };
        }

        std::unique_ptr<RenderUnit::Base> RU;
    };
}
