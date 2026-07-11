/**
 * @file Inspector.ixx
 * @brief 编辑器UI
 * @version 2.0
 * @author Cheyne Xie
 * @date 2024/10/16
 */

module;
#include "imgui/imgui.h"
export module CEngine.EditorUI:Inspector;
import :SceneTreeBrowser;
import :Inspector_Node;
import :Inspector_Node3D;
import :Inspector_RenderUnit;
import :Inspector_Light;
import :Inspector_Camera;
import :Inspector_Atmosphere3D;
import std;
import CEngine.Node;
import CEngine.Render;

namespace CEngine {

    export void DisplayInspector(const SceneTreeBrowser &scene_tree_browser) {
        const auto node = scene_tree_browser.NodeSelected;
        if (node == nullptr || !node->IsValid()) {
            ImGui::Text("Select A Node First");
            return;
        }
        ProcessNode(node);

        if (node->IsType(NodeType::Node3D))
            ProcessNode3D(static_cast<Node3D*>(node));

        switch (node->GetType()) {
            case NodeType::Camera3D: ProcessCamera(static_cast<Camera*>(static_cast<Camera3D*>(node))); break;
            case NodeType::Light3D: ProcessLight(static_cast<Light3D*>(node)->GetLight()); break;
            case NodeType::RenderUnit3D: ProcessRenderUnit(static_cast<RenderUnit3D*>(node)->GetRU()); break;
            case NodeType::Atmosphere3D: ProcessAtmosphere3D(static_cast<Atmosphere3D*>(node)); break;
            default: break;
        }  
    }
}
