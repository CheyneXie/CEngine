module;
export module CEngine.Node;
export import :Node;
export import :Node3D;
export import :RenderUnit3D;
export import :PBR3D;
export import :Camera3D;
export import :Behaviour;
export import :BehaviourFactory;

namespace CEngine {
    export enum RenderType {
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