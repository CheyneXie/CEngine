/**
 * @file UI.ixx
 * @brief UI总模块
 * @version 1.0
 * @author Chaim
 * @date 2024/10/14
 */

module;
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
export module CEngine.UI;
import CEngine.Base;
import CEngine.Logger;

namespace CEngine {
    export class UI : public Object {
    public:
        explicit UI(GLFWwindow *window) : Window(window) {
        }

        static void Destroy() {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }

        /// 初始化窗口（在创建GLFWwindow之后）
        virtual void InitUI() {
            LogI(TAG) << "初始化UI...";
            ImGui::CreateContext();
            ImGuiIO &io = ImGui::GetIO();
            (void) io;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
            // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
            io.Fonts->AddFontFromFileTTF("Roboto-Medium.ttf", 13.0f, nullptr, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
            ImGui::StyleColorsDark();
            // ImGui::StyleColorsLight();
            ImGui_ImplGlfw_InitForOpenGL(Window, true);
            ImGui_ImplOpenGL3_Init("#version 430");
        }

        /// 刷新显示UI（在Process中）
        virtual void ProcessUI() {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            // ImGui::ShowDemoWindow(nullptr);
            DrawUI();
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        virtual void DrawUI() {
        }

    protected:
        GLFWwindow *Window;
    };
}
