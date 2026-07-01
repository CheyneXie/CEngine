/**
 * @file Postprocessing.ixx
 * @author 后处理
 * @brief 基础渲染单元
 * @version 1.0
 * @date 2026-07-01
 * 
 */

// TODO

module;
export module CEngine.RenderUnit:Postprocessing;
import :Base;
import CEngine.Render;

namespace CEngine::RenderUnit {
    export class Postprocessing final : public Base {
    public:
        Type GetType() override { return Type::Postprocessing; }

        static std::unique_ptr<Postprocessing> Create() {
            return std::unique_ptr<Postprocessing>(new Postprocessing());
        }

    protected:
        Postprocessing() : Base(nullptr) {
            shader_program_names.push_back("Postprocessing");
        }
    };
}
