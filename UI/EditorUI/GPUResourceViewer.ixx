/**
 * @file GPUResourceViewer.ixx
 * @brief GPU资源查看器
 * @version 1.0
 * @author Chaim
 * @date 2024/10/17
 */

module;
#include "glad/glad.h"
#include "imgui/imgui.h"
export module CEngine.EditorUI:GPUResourceViewer;
import std;
import CEngine.Render;
import CEngine.Utils;

namespace CEngine {
    const char *GetOpenGLTextureFormatName(int format);

    export class GPUResourceViewer {
    public:
        ShaderProgram *SelectedShaderProgram = nullptr;
        Texture *SelectedTexture = nullptr;

        GPUResourceViewer() = default;

        void ShowGPUResourceViewer(bool *isOpen, const float window_width, const float window_height) {
            ImGui::SetNextWindowSize(ImVec2(std::clamp(window_width / 2, 400.f, 1000.f), std::clamp(window_height / 2, 300.f, 800.f)), ImGuiCond_Once);
            ImGui::Begin("GPU Resource Viewer", isOpen);
            if (ImGui::BeginTabBar("##tabs#GPUResourceViewer")) {
                if (ImGui::BeginTabItem("Shader Program")) {
                    ImGui::BeginChild("##ShaderProgram#List", ImVec2(ImGui::GetContentRegionAvail().x * 0.2f, 0), ImGuiWindowFlags_NoResize);
                    if (ImGui::Button("Reload All Shader", ImVec2(-FLT_MIN, 0))) {
                        ShaderManager::LoadShaderProgram();
                    }
                    if (ImGui::BeginListBox("##ShaderProgram#ListBox", ImVec2(-FLT_MIN, -FLT_MIN))) {
                        for (auto &[name, shader]: ShaderProgram::All_Instances)
                            if (ImGui::Selectable(name.c_str(), shader == SelectedShaderProgram))
                                SelectedShaderProgram = shader;
                        ImGui::EndListBox();
                    }
                    ImGui::EndChild();
                    ImGui::SameLine();
                    ImGui::BeginChild("##ShaderProgram#Detail", ImVec2(0, 0), ImGuiWindowFlags_NoResize);
                    if (SelectedShaderProgram != nullptr) {
                        ImGui::SeparatorText("Base Info");
                        if (ImGui::BeginTable("##ShaderProgram#Detail#Base", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
                            ImGui::TableSetupColumn("Name");
                            ImGui::TableSetupColumn("ShaderProgram ID");
                            ImGui::TableHeadersRow();
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text(SelectedShaderProgram->getName().c_str());
                            ImGui::TableNextColumn();
                            ImGui::Text(std::to_string(SelectedShaderProgram->getShaderProgramID()).c_str());
                            ImGui::EndTable();
                        }
                        ImGui::SeparatorText("Uniforms List");
                        if (ImGui::BeginTable("##ShaderProgram#Detail#Uniforms", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
                            ImGui::TableSetupColumn("Name");
                            ImGui::TableSetupColumn("Type");
                            ImGui::TableHeadersRow();
                            for (auto &[type, name]: SelectedShaderProgram->getUniformsList()) {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                ImGui::Text(name.c_str());
                                ImGui::TableNextColumn();
                                ImGui::Text(ShaderUniformVar::TypeToString(type));
                            }
                            ImGui::EndTable();
                        }
                    } else {
                        ImGui::Text("Please select a item.");
                    }
                    ImGui::EndChild();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Texture")) {
                    ImGui::BeginChild("##Texture#List", ImVec2(ImGui::GetContentRegionAvail().x * 0.2f, 0), ImGuiWindowFlags_NoResize);
                    if (ImGui::Button("Load New Texture", ImVec2(-FLT_MIN, 0))) {
                        Texture::FromFile(Utils::ShowOpenFileDialog().string().c_str());
                    }
                    if (ImGui::BeginListBox("##Texture#ListBox", ImVec2(-FLT_MIN, -FLT_MIN))) {
                        for (auto &[name, tex]: Texture::All_Instances)
                            if (ImGui::Selectable(name.c_str(), tex == SelectedTexture))
                                SelectedTexture = tex;
                        ImGui::EndListBox();
                    }
                    ImGui::EndChild();
                    ImGui::SameLine();
                    ImGui::BeginChild("##Texture#Detail", ImVec2(0, 0), ImGuiWindowFlags_NoResize);
                    if (SelectedTexture != nullptr) {
                        ImGui::SeparatorText("Base Info");
                        if (ImGui::BeginTable("##Texture#Detail#Base", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
                            ImGui::TableSetupColumn("Name");
                            ImGui::TableSetupColumn("Value");
                            ImGui::TableHeadersRow();
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("MD5");
                            ImGui::TableNextColumn();
                            ImGui::Text(SelectedTexture->getMd5().c_str());
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Texture ID");
                            ImGui::TableNextColumn();
                            ImGui::Text(std::to_string(SelectedTexture->getTextureID()).c_str());
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Data Format");
                            ImGui::TableNextColumn();
                            ImGui::Text(GetOpenGLTextureFormatName(SelectedTexture->getDataFormat()));
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Internal Format");
                            ImGui::TableNextColumn();
                            ImGui::Text(GetOpenGLTextureFormatName(SelectedTexture->getInternalFormat()));
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Size");
                            ImGui::TableNextColumn();
                            ImGui::Text("%d x %d", SelectedTexture->getWidth(), SelectedTexture->getHeight());
                            ImGui::EndTable();
                        }
                        ImGui::SeparatorText("Preview");
                        auto id = SelectedTexture->getTextureID();
                        ImGui::Image(static_cast<ImTextureID>(SelectedTexture->getTextureID()), ImVec2(SelectedTexture->getWidth(), SelectedTexture->getHeight()));
                    } else {
                        ImGui::Text("Please select a item.");
                    }
                    ImGui::EndChild();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Mesh")) {
                    ImGui::Text("Mesh count: %u", Mesh::All_Instances.size());
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::End();
        }
    };

    const char *GetOpenGLTextureFormatName(int format) {
        switch (format) {
            case GL_RGB: return "GL_RGB";
            case GL_RGB8: return "GL_RGB8";
            case GL_RGBA: return "GL_RGBA";
            case GL_RGBA8: return "GL_RGBA8";
            case GL_RED: return "GL_RED";
            case GL_R8: return "GL_R8";
            case GL_RG: return "GL_RG";
            case GL_RG8: return "GL_RG8";
            default: return "Unknown";
        }
    }
}
