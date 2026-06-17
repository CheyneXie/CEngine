/**
 * @file EventBus.ixx
 * @author Cheyne Xie
 * @brief 事件总线
 * @version 1.0
 * @date 2026-06-16
 */

module;
export module CEngine.EventBus;
import std;
import CEngine.Base;

namespace CEngine {
    /**
     * @brief 事件总线类
     */
    export class EventBus {
    public:
        // ===================== Engine =====================
        void ExecDeferred(std::function<void()> f, bool pre_process = false) {
            if (pre_process) {
                auto idx = EnginePreProcess.GetNextIdx();
                EnginePreProcess += [f = std::move(f), idx](double DeltaTime) {
                    f();
                    EventBus().EnginePreProcess -= idx;
                };
            } else {
                auto idx = EnginePostProcess.GetNextIdx();
                EnginePostProcess += [f = std::move(f), idx](double DeltaTime) {
                    f();
                    EventBus().EnginePostProcess -= idx;
                };
            }
        }

        /**
        * @brief 窗口大小被改变
        * @param window GLFWwindow* 对象
        * @param width 窗口宽度
        * @param height 窗口高度
        */
        Event<void(void *window, int width, int height)> WindowResized;
        /**
         *  @brief 引擎准备就绪
         */
        Event<void()> EngineReady;
        /**
         *  @brief 引擎退出时
         */
        Event<void()> EngineDestroy;
        /**
         * @brief 引擎帧处理(前)
         * @param DeltaTime 上一帧处理用时(ms)
        */
        Event<void(double DeltaTime)> EnginePreProcess;
        /**
         * @brief 引擎帧处理(后)
         * @param DeltaTime 处理用时(ms)
        */
        Event<void(double DeltaTime)> EnginePostProcess;

        /**
         * @brief 相机激活/变更活动相机
         * @param camera Camera对象
         */
        Event<void(void *camera)> CameraActivated;
        /**
         * @brief 着色器程序重新加载
         */
        Event<void()> ShaderReloaded;

        // ===================== Input ======================

        /**
         * @brief 键盘事件
         * @param window GLFWwindow* 对象
         * @param key 语义化的按键码（如 GLFW_KEY_W），受键盘布局影响
         * @param scancode 操作系统相关的物理按键扫描码，与键盘布局无关
         * @param action 按键的动作状态，取值包括 GLFW_PRESS（按下）、GLFW_RELEASE（释放）或 GLFW_REPEAT（长按重复）
         * @param mods 修饰键状态，表示按键时是否同时按下了 Shift、Ctrl、Alt 等组合键
         */
        Event<void(void *window, int key, int scancode, int action, int mods)> KeyEvent;

        /**
         * @brief 鼠标移动事件
         * @param window GLFWwindow* 对象
         * @param xpos 鼠标光标相对于窗口左上角的 X 轴坐标（单位为屏幕像素）
         * @param ypos 鼠标光标相对于窗口左上角的 Y 轴坐标（单位为屏幕像素）
         */
        Event<void(void *window, double xpos, double ypos)> MouseMoveEvent;

        /**
         * @brief 鼠标点击事件
         * @param window GLFWwindow* 对象
         * @param key 被按下的鼠标按键（如 GLFW_MOUSE_BUTTON_LEFT 代表左键）
         * @param action 按键的动作状态，取值包括 GLFW_PRESS（按下）、GLFW_RELEASE（释放）或 GLFW_REPEAT（长按重复）
         * @param mods 修饰键状态，表示按键时是否同时按下了 Shift、Ctrl、Alt 等组合键
         */
        Event<void(void *window, int key, int action, int mods)> MouseButtonEvent;

        /**
         * @brief 鼠标移动事件
         * @param window GLFWwindow* 对象
         * @param xpos 鼠标滚轮在水平方向上的滚动偏移量
         * @param ypos 鼠标滚轮在垂直方向上的滚动偏移量
         */
        Event<void(void *window, double xpos, double ypos)> MouseScrollEvent;

        // ==================================================

        EventBus(const EventBus&) = delete;
        EventBus& operator=(const EventBus&) = delete;
        EventBus(EventBus&&) = delete;
        EventBus& operator=(EventBus&&) = delete;

        static EventBus* Get() {
            static std::unique_ptr<EventBus> instance = std::unique_ptr<EventBus>(new EventBus());
            return instance.get();
        }

    private:
        EventBus() = default;
    };
    export inline auto& EventBus() {
        return *EventBus::Get();
    }
}