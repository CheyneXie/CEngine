/**
 * @file Atmosphere3D.ixx
 * @brief 氛围盒
 * @version 1.0
 * @author Cheyne Xie
 * @date 2026/07/01
 */

module;
#include <glm/glm.hpp>
export module CEngine.Node:Atmosphere3D;
import :Node3D;
import :RenderUnit3D;
import CEngine.Render;

// TODO

namespace CEngine {
    /**
     * @brief 后处理
     * @remark RenderUnit3D + IBL
     */
    export class Atmosphere3D : public RenderUnit3D {
    public:
        NodeType GetType() override { return NodeType::Atmosphere3D; }
        bool IsType(NodeType type) override { return RenderUnit3D::IsType(type) || type == NodeType::Atmosphere3D; }

        static Atmosphere3D *Create() {
            return new Atmosphere3D();
        }

        RenderUnit::Postprocessing *GetRU() {
            return static_cast<RenderUnit::Postprocessing*>(RenderUnit3D::GetRU());
        }

        void setBound(glm::vec3 bound) { Bound = bound; }
        glm::vec3 getBound() const { return Bound; }

        IBL& getIBL() { return mIBL; }
        
    protected:
        Atmosphere3D() : RenderUnit3D(RenderUnit::Postprocessing::Create()) {
        }

        IBL mIBL;

        glm::vec3 Bound = {1, 1, 1};
    };
}


