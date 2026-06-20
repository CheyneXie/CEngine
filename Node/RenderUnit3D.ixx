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
        const char *GetTypeName() override {
            return "RenderUnit3D";
        }

        static RenderUnit3D *Create(std::unique_ptr<RenderUnit> ru) {
            return new RenderUnit3D(std::move(ru));
        }

        RenderUnit *GetRU() {
            return RU.get();
        }

    protected:
        RenderUnit3D(std::unique_ptr<RenderUnit> ru) : RU(std::move(ru)) {
        }

        std::unique_ptr<RenderUnit> RU;
    };
}
