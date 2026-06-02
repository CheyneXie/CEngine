/**
 * @file SceneTreeBrowser.ixx
 * @brief 场景树浏览器
 * @version 1.0
 * @author Chaim
 * @date 2024/10/16
 */

module;
#include "imgui/imgui.h"
export module CEngine.EditorUI:SceneTreeBrowser;
import std;
import CEngine.Engine;
import CEngine.Node;

namespace CEngine {
    export class SceneTreeBrowser {
    public:
        Node *NodeSelected = nullptr;

        SceneTreeBrowser() = default;

        void RefreshCache() {
            NodeIsFolding.clear();
        }

        void DisplaySceneTreeBrowser() {
            std::stack<std::pair<Node *, int> > stack;
            for (auto node: Engine::GetIns()->getRoot()->GetChildren())
                stack.push({node, 0});
            while (!stack.empty()) {
                auto [node, tab] = stack.top();
                stack.pop();
                if (!NodeIsFolding.contains(node)) {
                    NodeIsFolding[node] = true;
                }
                std::string node_name;
                if (node->GetChildCount() > 0) {
                    if (NodeIsFolding[node]) {
                        node_name = std::format("{}- ({}) {}", std::string(tab, ' '), node->GetTypeName(), node->getName());
                        for (auto child: node->GetChildren())
                            stack.push({child, tab + 2});
                    } else {
                        node_name = std::format("{}+ ({}) {}", std::string(tab, ' '), node->GetTypeName(), node->getName());
                    }
                } else {
                    node_name = std::format("{}({}) {}", std::string(tab, ' '), node->GetTypeName(), node->getName());
                }
                if (ImGui::Selectable(node_name.c_str(), node == NodeSelected)) {
                    if (node != NodeSelected)
                        NodeSelected = node;
                    else
                        NodeSelected = nullptr;
                }
                if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered()) {
                    NodeIsFolding[node] = !NodeIsFolding[node];
                }
            }
        }

    private:
        std::unordered_map<Node *, bool> NodeIsFolding;
    };
}
