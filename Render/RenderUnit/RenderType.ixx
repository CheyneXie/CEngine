/**
 * @file RenderType.ixx
 * @author Cheyne Xie
 * @brief 渲染单元类型
 * @version 1.0
 * @date 2026-06-23
 * 
 */

module;
export module CEngine.RenderUnit:RenderType;
import std;

 namespace CEngine {
    export enum class RenderType {
        Base,
        PBR,
        Deferred_PBR,
        Count // 哨兵值，代表成员总数
    };

    // For UI
    export std::vector<std::pair<RenderType, const char*>> RenderTypeAndName = {
        {RenderType::Base, "Base"},
        {RenderType::PBR, "PBR"},
        {RenderType::Deferred_PBR, "Deferred PBR"}
    };
}