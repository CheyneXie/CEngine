/**
 * @file Type.ixx
 * @author Cheyne Xie
 * @brief 灯光类型
 * @version 1.0
 * @date 2026-06-24
 * 
 */

module;
export module CEngine.Light:Type;
import std;

 namespace CEngine::Light {
    export enum class Type {
        Directional,
        Point,
        Spot,
        Count // 哨兵值，代表成员总数
    };

    // For UI
    export std::vector<std::pair<Type, const char*>> TypeAndName = {
        {Type::Directional, "Directional Light"},
        {Type::Point, "Point Light"},
        {Type::Spot, "Spot Light"}
    };

    export const char* TypeToName(Type type) {
        switch (type) {
            case Type::Directional: return "Directional Light";
            case Type::Point: return "Point Light";
            case Type::Spot: return "Spot Light";
            case Type::Count: return "Error";
        }
    }
}