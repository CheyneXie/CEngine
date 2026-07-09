/**
 * @file Type.ixx
 * @author Cheyne Xie
 * @brief 渲染单元类型
 * @version 1.0
 * @date 2026-06-23
 * 
 */

module;
export module CEngine.RenderUnit:Type;
import std;

 namespace CEngine::RenderUnit {
    export enum class Type {
        Base,
        PBR,
        Deferred_PBR,
        Count // 哨兵值，代表成员总数
    };

    // For UI
    export constexpr std::array<std::pair<Type, const char*>, 3> TypeAndName = {{
        {Type::Base, "Base"},
        {Type::PBR, "PBR"},
        {Type::Deferred_PBR, "Deferred PBR"}
    }};

    export const char* GetTypeName(Type type) {
        switch (type) {
            case Type::Base: return "Base";
            case Type::PBR: return "PBR";
            case Type::Deferred_PBR: return "Deferred_PBR";
            case Type::Count: return "Error";
        }
    }
}