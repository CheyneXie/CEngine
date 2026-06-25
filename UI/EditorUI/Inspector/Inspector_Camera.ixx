/**
 * @file Inspector_Camera.ixx
 * @author Cheyne Xie
 * @brief 
 * @version 1.0
 * @date 2026-06-25
 * 
 */

module;
#include "imgui/imgui.h"
export module CEngine.EditorUI:Inspector_Camera;
import CEngine.Render;
import std;

namespace CEngine {
    void ProcessCamera(Camera* camera) {
        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
            bool activated = camera->IsActivated();
            if (ImGui::Checkbox("Activate", &activated)) {
                camera->Activate();
            }
            auto fov = camera->getFOV();
            if (ImGui::DragFloat("FOV", &fov, 0.1, 0, FLT_MAX, "%.3f")) {
                camera->setFOV(fov);
            }
            auto AspectRatio = camera->getAspectRatio();
            if (ImGui::DragFloat("Aspect Ratio", &AspectRatio, 0.1, 0, FLT_MAX, "%.3f")) {
                camera->setAspectRatio(AspectRatio);
            }
            auto ClippingPlane = camera->getClippingPlane();
            float ClippingPlane_Float[2] = {ClippingPlane.first, ClippingPlane.second};
            if (ImGui::DragFloat2("Clipping Plane", ClippingPlane_Float, 10.f, 0)) {
                camera->setClippingPlane(ClippingPlane_Float[0], ClippingPlane_Float[1]);
            }
        }

    }
}