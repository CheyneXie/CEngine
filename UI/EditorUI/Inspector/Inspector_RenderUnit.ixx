/**
 * @file Inspector_RenderUnit.ixx
 * @author Cheyne Xie
 * @brief 
 * @version 1.0
 * @date 2026-06-25
 * 
 */

module;
#include <climits>
#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui.h"
export module CEngine.EditorUI:Inspector_RenderUnit;
import CEngine.Render;
import CEngine.RenderUnit;
import std;

namespace CEngine {
    void ProcessMaterial(Material& material) {
        if (ImGui::CollapsingHeader("PBR", ImGuiTreeNodeFlags_DefaultOpen)) {
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

    export void ProcessRenderUnit(RenderUnit::Base *ru) {
        if (ImGui::CollapsingHeader("RenderUnit", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::TreeNodeEx("Shader Uniforms Override", ImGuiTreeNodeFlags_DefaultOpen)) {
                for (auto &[name,suv]: ru->getUniforms()) {
                    const auto type = suv.GetType();
                    ImGui::BulletText("%s", name.data());
                    ImGui::PushID(name.data());
                    if (type == ShaderUniformVar::Type::INT) {
                        auto v = suv.GetValue<int>();
                        if (ImGui::DragInt("int", &v, 1, -INT_MAX, INT_MAX, "%d")) {
                            suv.SetValue(v);
                        }
                    } else if (type == ShaderUniformVar::Type::UINT) {
                        auto v = static_cast<int>(suv.GetValue<unsigned int>());
                        if (ImGui::DragInt("uint", &v, 1, 0, UINT_MAX, "%d", ImGuiSliderFlags_ClampOnInput)) {
                            suv.SetValue(static_cast<unsigned int>(v));
                        }
                    } else if (type == ShaderUniformVar::Type::FLOAT) {
                        auto v = suv.GetValue<float>();
                        if (ImGui::DragFloat("float", &v, 0.1, -FLT_MAX, FLT_MAX, "%.3f")) {
                            suv.SetValue(v);
                        }
                    } else if (type == ShaderUniformVar::Type::DOUBLE) {
                        auto v = static_cast<float>(suv.GetValue<double>());
                        if (ImGui::DragFloat("float", &v, 0.1, -FLT_MAX, FLT_MAX, "%.3f")) {
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
                        for (const auto t: Texture::Get() | std::views::values) {
                            if (t == tex) {
                                v = index;
                                break;
                            }
                            ++index;
                        }
                        auto it = Texture::Get().cbegin();
                        if (ImGui::Combo("sampler2D", &v, [](void *data, const int idx)-> const char *{
                            if (idx == 0) return "None";
                            const auto it_ptr = static_cast<std::unordered_map<std::string, Texture *>::const_iterator *>(data);
                            if (it_ptr == nullptr) return "Error";
                            return std::next(*it_ptr, idx - 1)->first.data();
                        }, &it, Texture::Num() + 1)) {
                            if (v >= 1) suv.SetValue(std::next(it, v - 1)->second);
                        }
                    } else {
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Have not implemented.");
                    }
                    ImGui::PopID();
                }
                ImGui::Separator();
                int v = 0;
                for (auto& name : ru->getShaderProgramNames()) {
                    auto sp = ShaderProgram::Get(name);
                    auto it = sp->getUniformsList().cbegin();
                    auto sz = sp->getUniformsList().size();
                    if (ImGui::Combo("Add Override", &v, [](void *data, const int idx)-> const char *{
                        if (idx == 0) return "Add Override";
                        const auto it_ptr = static_cast<std::vector<std::pair<ShaderUniformVar::Type, std::string> >::const_iterator *>(data);
                        if (it_ptr == nullptr) return "Error";
                        return std::next(*it_ptr, idx - 1)->second.c_str();
                    }, &it, sz + 1)) {
                        if (v >= 1) {
                            const auto item = std::next(it, v - 1);
                            ru->SetShaderUniform(item->second, item->first);
                        }
                    }
                }
                ImGui::TreePop();
            }
        }
        // Material
        switch (ru->GetType()) {
            case RenderUnit::Type::PBR: ProcessMaterial(static_cast<RenderUnit::PBR*>(ru)->getMaterial()); break;
            case RenderUnit::Type::Deferred_PBR: ProcessMaterial(static_cast<RenderUnit::Deferred*>(ru)->getMaterial()); break;
            default: break;
        }
    }
}