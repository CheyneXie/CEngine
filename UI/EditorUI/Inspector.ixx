/**
 * @file Inspector.ixx
 * @brief 编辑器UI
 * @version 1.0
 * @author Chaim
 * @date 2024/10/16
 */

module;
#include <climits>
#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui.h"
export module CEngine.EditorUI:Inspector;
import :SceneTreeBrowser;
import std;
import CEngine.Base;
import CEngine.Node;
import CEngine.Logger;
import CEngine.Utils;
import CEngine.Render;

namespace CEngine {
    void ProcessNode(Node *node) {
        if (ImGui::CollapsingHeader("Node", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::TreeNodeEx("Info", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::BeginDisabled();
                ImGui::InputText("Type", const_cast<char *>(node->GetTypeName()), 16, ImGuiInputTextFlags_ReadOnly);
                ImGui::EndDisabled();
                ImGui::Separator();
                char Name[128];
                const auto name_str = node->getName();
                name_str.copy(Name, name_str.length());
                Name[name_str.length()] = '\0';
                if (ImGui::InputText("Name", Name, IM_ARRAYSIZE(Name), ImGuiInputTextFlags_EnterReturnsTrue))
                    node->setName(Name);
                ImGui::TreePop();
            }
        }
    }

    void ProcessNode3D(Node3D *node3d) {
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
                if (ImGui::DragFloat3("Rotation (L)", rot_l, 0.01f)) {
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

    void ProcessRenderUnit3D(RenderUnit3D *ru3d) {
        if (ImGui::CollapsingHeader("RenderUnit3D", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::TreeNodeEx("Shader Uniforms Override", ImGuiTreeNodeFlags_DefaultOpen)) {
                for (auto &[name,suv]: ru3d->getUniforms()) {
                    const auto type = suv.GetType();
                    ImGui::BulletText(name.c_str());
                    ImGui::PushID(name.c_str());
                    if (type == ShaderUniformVar::Type::INT) {
                        auto v = suv.GetValue<int>();
                        if (ImGui::DragInt("int", &v, 1, -INT_MAX, INT_MAX, "%d")) {
                            suv.SetValue(v);
                        }
                    } else if (type == ShaderUniformVar::Type::UINT) {
                        auto v = static_cast<int>(suv.GetValue<unsigned int>());
                        if (ImGui::DragInt("uint", &v, 1, 0, INT_MAX, "%d", ImGuiSliderFlags_ClampOnInput)) {
                            suv.SetValue(static_cast<unsigned int>(v));
                        }
                    } else if (type == ShaderUniformVar::Type::FLOAT) {
                        auto v = suv.GetValue<float>();
                        if (ImGui::DragFloat("float", &v, 0.1, -FLT_MAX, FLT_MAX, ".3f")) {
                            suv.SetValue(v);
                        }
                    } else if (type == ShaderUniformVar::Type::DOUBLE) {
                        auto v = static_cast<float>(suv.GetValue<double>());
                        if (ImGui::DragFloat("float", &v, 0.1, -FLT_MAX, FLT_MAX, ".3f")) {
                            suv.SetValue(static_cast<double>(v));
                        }
                    } else if (type == ShaderUniformVar::Type::VEC2) {
                        auto v = suv.GetValue<glm::vec2>();
                        if (ImGui::DragFloat2("vec2", glm::value_ptr(v), 0.1, -FLT_MAX, FLT_MAX, "%.3f")) {
                            suv.SetValue(v);
                        }
                    } else if (type == ShaderUniformVar::Type::VEC3) {
                        auto v = suv.GetValue<glm::vec3>();
                        if (ImGui::ColorEdit3("vec3", glm::value_ptr(v), ImGuiColorEditFlags_Float)) {
                            suv.SetValue(v);
                        }
                    } else if (type == ShaderUniformVar::Type::VEC4) {
                        auto v = suv.GetValue<glm::vec4>();
                        if (ImGui::ColorEdit4("vec4", glm::value_ptr(v), ImGuiColorEditFlags_Float)) {
                            suv.SetValue(v);
                        }
                    } else if (type == ShaderUniformVar::Type::SAMPLER2D) {
                        const auto tex = suv.GetValue<Texture *>();
                        int v = 0;
                        int index = 1;
                        for (const auto t: Texture::All_Instances | std::views::values) {
                            if (t == tex) {
                                v = index;
                                break;
                            }
                            ++index;
                        }
                        // 为了时索引为0时输出NULL，所有变量偏移1
                        if (ImGui::Combo("sampler2D", &v, [](void *data, const int idx)-> const char *{
                            if (idx == 0) return "NULL";
                            return std::next(Texture::All_Instances.cbegin(), idx - 1)->first.c_str();
                        }, nullptr, Texture::All_Instances.size() + 1)) {
                            if (v >= 1)
                                suv.SetValue(std::next(Texture::All_Instances.cbegin(), v - 1)->second);
                        }
                    } else {
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Have not implemented.");
                    }
                    ImGui::PopID();
                }
                ImGui::Separator();
                int v = 0;
                auto it = ru3d->getShaderProgram()->getUniformsList().cbegin();
                if (ImGui::Combo("Add Override", &v, [](void *data, const int idx)-> const char *{
                    if (idx == 0) return "Add Override";
                    const auto it_ptr = static_cast<std::unordered_set<std::pair<ShaderUniformVar::Type, std::string> >::const_iterator *>(data);
                    if (it_ptr == nullptr) return "NULL";
                    return std::next(std::forward<std::unordered_set<std::pair<ShaderUniformVar::Type, std::string> >::const_iterator>(*it_ptr),
                                     idx - 1)->second.c_str();
                }, &it, ru3d->getShaderProgram()->getUniformsList().size() + 1)) {
                    if (v > 1) {
                        const auto item = std::next(std::forward<std::unordered_set<std::pair<ShaderUniformVar::Type, std::string> >::const_iterator>(it),
                                                    v - 1);
                        ShaderUniformVar::Type type = item->first;
                        ru3d->SetShaderUniform(item->second, type);
                    }
                }
                ImGui::TreePop();
            }
        }
    }

    void ProcessPBR3D(PBR3D *pbr3d) {
        Material &material = pbr3d->getMaterial();
        if (ImGui::CollapsingHeader("PBR3D", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SeparatorText("Parameters");
                ImGui::Text("Emissive Intensity");
                if (ImGui::DragFloat("Float##emissive_intensity", &material.Parameters.EMISSIVE_INTENSITY, 0.01f, 0.0f, FLT_MAX, "%.3f")) material.UpdateParameters();
                ImGui::Text("Metallic");
                if (ImGui::DragFloat("Float##metallic", &material.Parameters.METALLIC, 0.01f, 0.0f, 1.0f, "%.3f")) material.UpdateParameters();
                ImGui::Text("Roughness");
                if (ImGui::DragFloat("Float##roughness", &material.Parameters.ROUGHNESS, 0.01f, 0.0f, 1.0f, "%.3f")) material.UpdateParameters();
                ImGui::Text("Opacity");
                if (ImGui::DragFloat("Float##opacity", &material.Parameters.OPACITY, 0.01f, 0.0f, 1.0f, "%.3f")) material.UpdateParameters();
                ImGui::Text("Diffuse Color");
                if (ImGui::ColorEdit4("Color4##diffuseColor", &material.Parameters.DIFFUSE_COLOR.r)) material.UpdateParameters();
                ImGui::Text("Emission Color");
                if (ImGui::ColorEdit4("Color4##emissionColor", &material.Parameters.EMISSION_COLOR.r)) material.UpdateParameters();
                ImGui::SeparatorText("Textures");
                for (auto &[type, value]: material.Textures) {
                    if (value.first == nullptr) continue;
                    ImGui::Checkbox(aiTextureTypeToString(type), &value.second);
                }
                ImGui::TreePop();
            }
        }
    }

    export void DisplayInspector(const SceneTreeBrowser &scene_tree_browser) {
        const auto node = scene_tree_browser.NodeSelected;
        if (node == nullptr || !node->IsValid()) {
            ImGui::Text("Select A Node First");
            return;
        }
        ProcessNode(node);

        if (const auto node3d = dynamic_cast<Node3D *>(node); node3d != nullptr && node3d->IsValid())
            ProcessNode3D(node3d);

        if (const auto ru3d = dynamic_cast<RenderUnit3D *>(node); ru3d != nullptr && ru3d->IsValid())
            ProcessRenderUnit3D(ru3d);

        if (const auto pbr3d = dynamic_cast<PBR3D *>(node); pbr3d != nullptr && pbr3d->IsValid())
            ProcessPBR3D(pbr3d);
    }
}
