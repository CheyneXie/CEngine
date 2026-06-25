/**
 * @file Inspector_Node3D.ixx
 * @author Cheyne Xie
 * @brief 
 * @version 1.0
 * @date 2026-06-25
 */

module;
#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui.h"
export module CEngine.EditorUI:Inspector_Node3D;
import CEngine.Base;
import CEngine.Node;
import CEngine.Utils;
import std;

namespace CEngine {
    export void ProcessNode3D(Node3D *node3d) {
        if (ImGui::CollapsingHeader("Node3D", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SeparatorText("Model");
                float pos_l[3];
                Utils::vec3_to_float3(node3d->GetPosition(), pos_l);
                if (ImGui::DragFloat3("Position (L)", pos_l, 0.01f)) {
                    node3d->SetPosition(glm::vec3(pos_l[0], pos_l[1], pos_l[2]));
                }
                float rot_l[3];
                Utils::vec3_to_float3(node3d->GetRotation().ToDegreesVec3XYZ(), rot_l);
                if (ImGui::DragFloat3("Rotation (L)", rot_l, 1.f)) {
                    node3d->SetRotation(EulerRotation::FromDegreesXYZ(rot_l[0], rot_l[1], rot_l[2]));
                }
                float scale_l[3];
                Utils::vec3_to_float3(node3d->GetScale(), scale_l);
                if (ImGui::DragFloat3("Scale (L)", scale_l, 0.01f, -FLT_MAX, FLT_MAX, "%.3f", ImGuiSliderFlags_ClampZeroRange)) {
                    node3d->SetScale(glm::vec3(scale_l[0], scale_l[1], scale_l[2]));
                }
                ImGui::SeparatorText("World");
                ImGui::BeginDisabled();
                ImGui::DragFloat3("Position (W)", const_cast<float *>(glm::value_ptr(node3d->GetWorldPosition())));
                ImGui::DragFloat3("Rotation (W)", const_cast<float *>(glm::value_ptr(node3d->GetWorldRotation().ToDegreesVec3XYZ())));
                ImGui::DragFloat3("Scale (W)", const_cast<float *>(glm::value_ptr(node3d->GetWorldScale())));
                ImGui::EndDisabled();
                ImGui::TreePop();
            }
        }
    }
}