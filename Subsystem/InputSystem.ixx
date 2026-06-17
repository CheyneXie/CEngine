/**
 * @file InputSystem.ixx
 * @author Cheyne Xie
 * @brief 输入系统
 * @version 1.0
 * @date 2026-06-17
 * 
 */

module;
#include <GLFW/glfw3.h>
export module CEngine.InputSystem;
import std;
import CEngine.EventBus;

namespace CEngine {
    export class InputSystem {
    public:
        static InputSystem *Get() {
            static std::unique_ptr<InputSystem> instance = std::unique_ptr<InputSystem>(new InputSystem());
            return instance.get();
        }

        static void Init(GLFWwindow *window) {
            Get()->window = window;
            glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
                EventBus().KeyEvent.Invoke(window, key, scancode, action, mods);
            });
            glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos, double ypos) {
                EventBus().MouseMoveEvent.Invoke(window, xpos, ypos);
            });
            glfwSetMouseButtonCallback(window, [](GLFWwindow *window, int key, int action, int mods) {
                EventBus().MouseButtonEvent.Invoke(window, key, action, mods);
            });
            glfwSetScrollCallback(window, [](GLFWwindow *window, double xpos, double ypos) {
                EventBus().MouseScrollEvent.Invoke(window, xpos, ypos);
            });
        }

        int Key(int key) const {
            return glfwGetKey(window, key);
        }

        int MouseButton(int button) const {
            return glfwGetMouseButton(window, button);
        }

        struct CursorPosData {
            double x;
            double y;
        };

        CursorPosData CursorPos() const {
            CursorPosData cp { 0, 0 };
            glfwGetCursorPos(window, &cp.x, &cp.y);
            return cp;
        }

        InputSystem(const InputSystem&) = delete;
        InputSystem& operator=(const InputSystem&) = delete;
        InputSystem(InputSystem&&) = delete;
        InputSystem& operator=(InputSystem&&) = delete;

    private:
        InputSystem() = default;

        GLFWwindow *window = nullptr;
    };
    export inline auto& InputSystem() {
        return *InputSystem::Get();
    }
}