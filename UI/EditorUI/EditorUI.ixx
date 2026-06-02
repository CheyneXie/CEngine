/**
 * @file EditorUI.ixx
 * @brief 编辑器UI
 * @version 1.0
 * @author Chaim
 * @date 2024/10/16
 */

module;
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
export module CEngine.EditorUI;
import :FileBrowser;
import :SceneTreeBrowser;
import :Inspector;
import :GPUResourceViewer;
import std;
import CEngine.UI;
import CEngine.Engine;
import CEngine.Logger;

namespace CEngine {
    export class EditorUI final : public UI {
    public:
        static const char *TAG;

        EditorUI(): UI(Engine::GetIns()->getWindow()) {
            file_browser.RefreshCache();
            scene_tree_browser.RefreshCache();
            Engine::GetIns()->Event_Process += [this](const double DeltaTime) {
                if (time_record > 1000) {
                    fps = frame_record / time_record * 1000;
                    frame_time = time_record / frame_record;
                    time_record = 0;
                    frame_record = 0;
                }
                time_record += DeltaTime;
                frame_record++;
            };
        }

        void DrawUI() override {
            // 窗口大小
            const auto [_w, _h] = Engine::GetIns()->GetScreenSize();
            const auto window_width = static_cast<float>(_w);
            const auto window_height = static_cast<float>(_h);
            // GPU Resource Viewer
            if (show_gpu_resource_viewer)
                gpu_resource_viewer.ShowGPUResourceViewer(&show_gpu_resource_viewer, window_width, window_height);
            // DemoWindow
            if (show_demo_window)
                ImGui::ShowDemoWindow(&show_demo_window);
            // Menu Bar
            if (ImGui::BeginMainMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("New Scene"))
                        Engine::GetIns()->getRoot()->RemoveAllChildren();
                    ImGui::MenuItem("Open Scene");
                    if (ImGui::MenuItem("Save Scene"))
                        Utils::ShowSaveFileDialog();
                    if (ImGui::MenuItem("Open Directory"))
                        if (const auto path = Utils::ShowSelectFolderDialog(); !path.empty())
                            file_browser.setRootPath(path);
                    if (ImGui::MenuItem("Refresh Directory"))
                        file_browser.RefreshCache();
                    if(ImGui::MenuItem("Exit"))
                        Engine::GetIns()->Exit();
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Edit")) {
                    ImGui::MenuItem("Undo");
                    ImGui::MenuItem("Redo");
                    ImGui::MenuItem("Cut");
                    ImGui::MenuItem("Copy");
                    ImGui::MenuItem("Paste");
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Scene")) {
                    if (ImGui::MenuItem("Refresh Scene"))
                        scene_tree_browser.RefreshCache();
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Tools")) {
                    if (ImGui::MenuItem("Left Panel", nullptr, show_left_panel))
                        show_left_panel = !show_left_panel;
                    if (ImGui::MenuItem("Right Panel", nullptr, show_right_panel))
                        show_right_panel = !show_right_panel;
                    if (ImGui::MenuItem("Bottom Bar", nullptr, show_bottom_bar))
                        show_bottom_bar = !show_bottom_bar;
                    if (ImGui::MenuItem("GPU Resource Viewer", nullptr, show_gpu_resource_viewer))
                        show_gpu_resource_viewer = !show_gpu_resource_viewer;
                    if (ImGui::MenuItem("ImGui Demo Window", nullptr, show_demo_window))
                        show_demo_window = !show_demo_window;
                    ImGui::MenuItem("Options");
                    ImGui::MenuItem("Settings");
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("More")) {
                    ImGui::MenuItem("Help");
                    ImGui::MenuItem("About");
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }

            if (show_left_panel) {
                // 左Panel
                ImGui::SetNextWindowPos(ImVec2(0, 20));
                ImGui::SetNextWindowSize(ImVec2(window_width * 0.15f, window_height - 20 - 30));
                if (ImGui::Begin("Left Panel", nullptr, ImGuiWindowFlags_NoMove)) {
                    // 场景树
                    if (ImGui::BeginChild("Scene Tree", ImVec2(0, ImGui::GetContentRegionAvail().y * 0.5f), ImGuiChildFlags_None)) {
                        if (ImGui::BeginTabBar("##tabs#Scene Tree")) {
                            if (ImGui::BeginTabItem("Scene Tree")) {
                                scene_tree_browser.DisplaySceneTreeBrowser();
                                ImGui::EndTabItem();
                            }
                            ImGui::EndTabBar();
                        }
                        ImGui::EndChild();
                    }

                    // 文件浏览器
                    if (ImGui::BeginChild("File Browser", ImVec2(0, 0), ImGuiChildFlags_None)) {
                        if (ImGui::BeginTabBar("##tabs#File Browser")) {
                            if (ImGui::BeginTabItem("File Browser")) {
                                file_browser.ShowDirectory(scene_tree_browser);
                                ImGui::EndTabItem();
                            }
                            ImGui::EndTabBar();
                        }
                        ImGui::EndChild();
                    }
                }
                ImGui::End();
            }

            if (show_right_panel) {
                // 右Panel
                ImGui::SetNextWindowPos(ImVec2(window_width * 0.8f, 20));
                ImGui::SetNextWindowSize(ImVec2(window_width * 0.2f, window_height - 20 - 30));
                if (ImGui::Begin("Right Panel", nullptr, ImGuiWindowFlags_NoMove)) {
                    if (ImGui::BeginTabBar("##tabs#Inspector")) {
                        if (ImGui::BeginTabItem("Inspector")) {
                            DisplayInspector(scene_tree_browser);
                            ImGui::EndTabItem();
                        }
                        ImGui::EndTabBar();
                    }
                }
                ImGui::End();
            }

            if (show_bottom_bar) {
                // 底栏
                ImGui::SetNextWindowPos(ImVec2(0, window_height - 30));
                ImGui::SetNextWindowSize(ImVec2(window_width, 30));
                if (ImGui::Begin("Info Window", nullptr,
                                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove)) {
                    ImGui::Text("  FPS: %.1f   |   Render Time: %.2fms   |  ", fps, frame_time);
                    ImGui::SameLine();
                    ImGui::Text("Window Size: %.0f x %.0f   |  ", window_width, window_height);
                }
                ImGui::End();
            }
        }

    private:
        int frame_record = 0;
        double fps = 0, frame_time = 0, time_record = 0;
        FileBrowser file_browser = FileBrowser(".");
        SceneTreeBrowser scene_tree_browser = SceneTreeBrowser();
        GPUResourceViewer gpu_resource_viewer = GPUResourceViewer();
        bool show_left_panel = true, show_right_panel = true, show_bottom_bar = true, show_gpu_resource_viewer = false, show_demo_window = false;
    };

    const char *EditorUI::TAG = "EditorUI";
}
