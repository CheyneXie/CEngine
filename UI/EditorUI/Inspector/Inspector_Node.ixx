/**
 * @file Node.ixx
 * @author Cheyne Xie
 * @brief Node 
 * @version 0.1
 * @date 2026-06-25
 * 
 */

module;
#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui.h"
export module CEngine.EditorUI:Inspector_Node;
import CEngine.Node;
import std;

namespace CEngine {
    export void ProcessNode(Node *node) {
        if (const auto behaviour = node->GetBehaviour(); behaviour != nullptr) {
            if (ImGui::CollapsingHeader("Behaviour Params", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SeparatorText(behaviour->GetName().c_str());
                for (auto& [name, param] : behaviour->GetParams()) {
                    ImGui::BulletText("%s", name.data());
                    ImGui::PushID(name.data());
                    if (param->type == typeid(float)) {
                        auto p = static_cast<Behaviour::ParamHolder<float>*>(param.get());
                        auto v = p->getter();
                        if (ImGui::DragFloat("float", &v, 0.1, -FLT_MAX, FLT_MAX, "%.3f")) {
                            p->setter(v);
                        }
                    } else if (param->type == typeid(double)) {
                        auto p = static_cast<Behaviour::ParamHolder<double>*>(param.get());
                        auto v = static_cast<float>(p->getter());
                        if (ImGui::DragFloat("float", &v, 0.1, -FLT_MAX, FLT_MAX, "%.3f")) {
                            p->setter(v);
                        }
                    } else if (param->type == typeid(glm::vec2)) {
                        auto p = static_cast<Behaviour::ParamHolder<glm::vec2>*>(param.get());
                        auto v = p->getter();
                        if (ImGui::DragFloat2("vec2", glm::value_ptr(v), 0.1, -FLT_MAX, FLT_MAX, "%.3f")) {
                            p->setter(v);
                        }
                    } else if (param->type == typeid(glm::vec3)) {
                        auto p = static_cast<Behaviour::ParamHolder<glm::vec3>*>(param.get());
                        auto v = p->getter();
                        if (ImGui::ColorEdit3("vec3", glm::value_ptr(v), ImGuiColorEditFlags_Float)) {
                            p->setter(v);
                        }
                    } else if (param->type == typeid(glm::vec4)) {
                        auto p = static_cast<Behaviour::ParamHolder<glm::vec4>*>(param.get());
                        auto v = p->getter();
                        if (ImGui::ColorEdit4("vec4", glm::value_ptr(v), ImGuiColorEditFlags_Float)) {
                            p->setter(v);
                        }
                    }
                    ImGui::PopID();
                }
            }
        }
        if (ImGui::CollapsingHeader("Node", ImGuiTreeNodeFlags_DefaultOpen)) {
            bool active = node->IsActive();
            if (ImGui::Checkbox("Active", &active)) {
                node->SetActive(active);
            }
            if (ImGui::TreeNodeEx("Info", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::BeginDisabled();
                ImGui::InputText("Type", const_cast<char *>(GetNodeTypeName(node->GetType())), 16, ImGuiInputTextFlags_ReadOnly);
                ImGui::EndDisabled();
                char Name[128];
                const auto name_str = node->getName();
                name_str.copy(Name, name_str.length());
                Name[name_str.length()] = '\0';
                if (ImGui::InputText("Name", Name, IM_ARRAYSIZE(Name), ImGuiInputTextFlags_EnterReturnsTrue))
                    node->setName(Name);
                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("Behaviour", ImGuiTreeNodeFlags_DefaultOpen)) {
                Behaviour* behaviour = node->GetBehaviour();
                std::string currentName = behaviour ? behaviour->GetName() : "None";
                if (ImGui::BeginCombo("Behaviour", currentName.c_str())) {
                    if (ImGui::Selectable("None", behaviour == nullptr)) {
                        node->SetBehaviour(nullptr);
                    }
                    for (const auto& name : BehaviourFactory::GetAllBehavioursName()) {
                        bool isSelected = (name == currentName);
                        if (ImGui::Selectable(name.data(), isSelected)) {
                            node->SetBehaviour(BehaviourFactory::CreateBehaviour(name));
                        }
                        if (isSelected) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                ImGui::TreePop();
            }
        }
    }
}