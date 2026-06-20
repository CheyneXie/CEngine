module;
export module CEngine.RenderUnit;
export import :Base;
export import :PBR;
export import :Deferred;

namespace CEngine {
    export enum class RenderType {
        Base,
        PBR,
        Deferred_Base,
        Deferred_PBR
    };

    // For UI
    export std::vector<std::pair<RenderType, const char*>> RenderTypeAndName = {
        {RenderType::Base, "Base"},
        {RenderType::PBR, "PBR"},
        {RenderType::Deferred_Base, "Deferred Base"},
        {RenderType::Deferred_PBR, "Deferred PBR"}
    };
}