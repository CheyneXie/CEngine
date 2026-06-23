/**
 * @file NodeType.ixx
 * @author Cheyne Xie
 * @brief Node 类型枚举
 * @version 1.0
 * @date 2026-06-23
 * 
 */

module;
export module CEngine.Node:NodeType;

namespace CEngine {
    export enum class NodeType {
        Node,
        Node3D,
        Camera3D,
        RenderUnit3D,
        Count // 哨兵值，代表成员总数
    };

    export const char *GetNodeTypeName(NodeType nt) {
        switch(nt) {
            case NodeType::Node: return "Node";
            case NodeType::Node3D: return "Node3D";
            case NodeType::Camera3D: return "Camera3D";
            case NodeType::RenderUnit3D: return "RenderUnit3D";
            default: return "Error NodeType";
        }
    }
}