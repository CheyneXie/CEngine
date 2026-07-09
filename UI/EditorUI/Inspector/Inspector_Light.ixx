/**
 * @file Inspector_Light.ixx
 * @author Cheyne Xie
 * @brief 
 * @version 1.0
 * @date 2026-06-25
 * 
 */

module;
#include "imgui/imgui.h"
export module CEngine.EditorUI:Inspector_Light;
import CEngine.Light;
import CEngine.Utils;
import std;

namespace CEngine {
    export void ProcessLight(Light::Base *light) {
        auto type = light->GetType();
        if (ImGui::CollapsingHeader(Light::TypeToName(type), ImGuiTreeNodeFlags_DefaultOpen)) {
            {
                auto illuminance = light->getIlluminance();
                if (ImGui::DragFloat("Illuminance", &illuminance, 0.1, -FLT_MAX, FLT_MAX, "%.3f")) {
                    light->setIlluminance(illuminance);
                }
                ImGui::ColorEdit3("Color", &(light->getColor().R));
            }
            if (type == Light::Type::Directional) {
                auto dl = static_cast<Light::Directional*>(light);
                ImGui::Checkbox("Set direction manually (does not follow rotation)", &dl->SetDirectionManually);
                ImGui::BeginDisabled(!dl->SetDirectionManually);
                float dir[3];
                Utils::vec3_to_float3(dl->getDirection(), dir);
                if (ImGui::DragFloat3("Direction", dir, 0.01f)) {
                    dl->setDirection({dir[0], dir[1], dir[2]});
                }
                ImGui::EndDisabled();
            } else if (type == Light::Type::Point) {
                auto pl = static_cast<Light::Point*>(light);
                auto ConstantFactor = pl->getConstantFactor();
                if (ImGui::DragFloat("Constant Factor", &ConstantFactor, 0.1, -FLT_MAX, FLT_MAX, "%.3f")) {
                    pl->setConstantFactor(ConstantFactor);
                }
                auto LinearFactor = pl->getLinearFactor();
                if (ImGui::DragFloat("Linear Factor", &LinearFactor, 0.1, -FLT_MAX, FLT_MAX, "%.3f")) {
                    pl->setLinearFactor(LinearFactor);
                }
                auto QuadraticFactor = pl->getQuadraticFactor();
                if (ImGui::DragFloat("Quadratic Factor", &QuadraticFactor, 0.1, -FLT_MAX, FLT_MAX, "%.3f")) {
                    pl->setQuadraticFactor(QuadraticFactor);
                }
            }
        }
    }
}