/**
 * @file CreatePanel.ixx
 * @author Cheyne Xie
 * @brief 
 * @version 1.0
 * @date 2026-06-25
 * 
 */

module;
#include "imgui/imgui.h"
export module CEngine.EditorUI:CreatePanel;
import :SceneTreeBrowser;
import CEngine.Engine;
import CEngine.Node;
import CEngine.Light;
import CEngine.RenderUnit;

namespace CEngine {
    export void DisplayCreatePanel(const SceneTreeBrowser &scene_tree_browser) {
        auto node = scene_tree_browser.NodeSelected;
        if (node == nullptr || !node->IsValid()) {
            node = Engine::GetIns()->getRoot();
        }

        constexpr int BtnHeight = 20;

        ImGui::SeparatorText("Node");
        if (ImGui::Button("Node", ImVec2(-1, BtnHeight))) {
            node->AddChild(Node::Create());
        }
        ImGui::Spacing();
        if (ImGui::Button("Node3D", ImVec2(-1, BtnHeight))) {
            node->AddChild(Node3D::Create());
        }
        ImGui::Spacing();
        ImGui::SeparatorText("Camera");
        if (ImGui::Button("Camera3D", ImVec2(-1, BtnHeight))) {
            node->AddChild(Camera3D::Create());
        }
        ImGui::Spacing();
        if (ImGui::Button("Camera3D (with Fly Camera Behaviour)", ImVec2(-1, BtnHeight))) {
            const auto camera = Camera3D::Create();
            camera->SetBehaviour(BehaviourFactory::CreateBehaviour("Fly Camera"));
            node->AddChild(camera);
            camera->Activate();
        }
        ImGui::Spacing();
        ImGui::SeparatorText("Light");
        if (ImGui::Button("Directional Light", ImVec2(-1, BtnHeight))) {
            // TODO 判断方向光唯一
            node->AddChild(Light3D::Create());
        }
        ImGui::Spacing();
        if (ImGui::Button("Point Light", ImVec2(-1, BtnHeight))) {
            node->AddChild(Light3D::Create(Light::Point::Create()));
        }
        ImGui::Spacing();
        ImGui::SeparatorText("Basic Shape");
        // 渲染模式选择
        static RenderUnit::Type currentRenderType = RenderUnit::Type::Base;
        int currentRenderTypeIdx = 0;
        for (size_t i = 0; i < RenderUnit::TypeAndName.size(); ++i) {
            if (RenderUnit::TypeAndName[i].first == currentRenderType) {
                currentRenderTypeIdx = static_cast<int>(i);
                break;
            }
        }
        if (ImGui::BeginCombo("Render Type", RenderUnit::TypeAndName[currentRenderTypeIdx].second)) 
        {
            for (size_t i = 0; i < RenderUnit::TypeAndName.size(); ++i) 
            {
                bool isSelected = (currentRenderTypeIdx == static_cast<int>(i));
                if (ImGui::Selectable(RenderUnit::TypeAndName[i].second, isSelected)) 
                {
                    currentRenderTypeIdx = static_cast<int>(i);
                    currentRenderType = RenderUnit::TypeAndName[i].first;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::Spacing();
        if (ImGui::Button("Panel", ImVec2(-1, BtnHeight))) {

        }
        ImGui::Spacing();
        if (ImGui::Button("Cube", ImVec2(-1, BtnHeight))) {

        }
        ImGui::Spacing();
        if (ImGui::Button("Sphere", ImVec2(-1, BtnHeight))) {

        }
        ImGui::Spacing();
    }
}