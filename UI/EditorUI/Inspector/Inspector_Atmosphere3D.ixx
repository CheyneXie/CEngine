/**
 * @file Inspector_Atmosphere3D.ixx
 * @author Cheyne Xie
 * @brief 
 * @version 1.0
 * @date 2026-07-11
 * 
 */

module;
#include "imgui/imgui.h"
export module CEngine.EditorUI:Inspector_Atmosphere3D;
import CEngine.Node;
import CEngine.Render;
import std;

namespace CEngine {
    void ProcessAtmosphere3D(Atmosphere3D* atm) {
        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
            auto ibl = IBL::GetActive();
            if (ibl == nullptr) {
                ImGui::Text("Something goes wrong. There is no activated ibl in the scene.");
                return;
            }
            ImGui::SeparatorText("HDRI");
            if (ImGui::Button("Use Built-In HDRI", ImVec2(-1, 20))) {
                ibl->LoadDefault();
            }
            if (ImGui::Button("Select HDRI File", ImVec2(-1, 20))) {
                // TODO
            }
            ImGui::SeparatorText("IBL Params");
            auto ibl_strength_d = ibl->getDiffuseIBLStrength();
            if (ImGui::DragFloat("Diffuse Strength", &ibl_strength_d, 0.01, 0, 10)) {
                ibl->setDiffuseIBLStrength(ibl_strength_d);
            }
            auto ibl_strength_s = ibl->getSpecularIBLStrength();
            if (ImGui::DragFloat("Specular Strength", &ibl_strength_s, 0.01, 0, 10)) {
                ibl->setSpecularIBLStrength(ibl_strength_s);
            }
        }
    }
}