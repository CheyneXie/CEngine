/**
 * @file Engine.ixx
 * @brief 引擎主模块
 * @version 1.0
 * @author Chaim
 * @date 2024/09/27
 */

module;
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
export module CEngine.Engine;
import std;
import CEngine.Base;
import CEngine.Logger;
import CEngine.Node;
import CEngine.Render;
import CEngine.Light;
import CEngine.RenderUnit;
import CEngine.Utils.RenderUtils;
import CEngine.UI;
import CEngine.PresetsLoader;
import CEngine.EventBus;
import CEngine.InputSystem;

namespace CEngine {
    /**
    * @class Engine
    * @brief CEngine引擎类
    * @remark 单例！\n
    */
    export class Engine final : Object {
    public:
        const static char *TAG;
        /**
        * 初始化引擎
        * @param width 窗口宽度
        * @param height 窗口高度
        * @param title 窗口标题
        * @return 窗口创建是否成功
        */
        static bool Init(const int width, const int height, const char *title);
        /**
        * 获得引擎实例
        */
        static Engine *GetIns();
        /**
        * 引擎主循环
        * @remark 堵塞型
        */
        void Loop();
        /**
        * 退出引擎主循环
        * @remark 仅标记为退出，不会立即退出引擎
        */
        void Exit() const;

        std::pair<int, int> GetScreenSize() const;

        /// @property RootNode
        Node3D *getRoot() const { return RootNode; }

        /// @property window
        GLFWwindow *getWindow() const { return window; }

        /// @property ui
        void setUI(UI *u) {
            if (ui == nullptr || !ui->IsValid())
                u->InitUI();
            ui = u;
        }

    private:
        Engine();
        /// @brief 窗口对象指针<code>GLFWwindow</code>
        GLFWwindow *window;
        /// @brief UI
        UI *ui;
        /// @brief 节点根目录
        Node3D *RootNode = Node3D::Create();
        /// 相机
        Camera *CurrentCamera;
        Camera3D *CurrentCamera3D;

        /**
        * 当引擎准备就绪时
        */
        void Ready();
        /**
        * 引擎帧处理函数
        * @return 处理用时(ms)
        */
        double Process(double DeltaTime);
        /**
        * 当引擎退出时
        */
        void Destroy();
    };

    const char *Engine::TAG = "引擎";

    Engine::Engine() {
        glfwInit();
        glfwWindowHint(GLFW_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_VERSION_MINOR, 3);
        RootNode->setName("Root");
    }

    bool Engine::Init(const int width, const int height, const char *title) {
        auto engine = Engine::GetIns();
        engine->window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (engine->window == nullptr) {
            LogE(TAG) << "创建窗口失败!";
            glfwTerminate();
            return false;
        }
        glfwMakeContextCurrent(engine->window);
        glfwSwapInterval(0); // 关闭垂直同步
        LogS(TAG) << "窗口创建成功.";

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
            LogE(TAG) << "GLAD加载失败!";
            glfwTerminate();
            return false;
        }
        LogS(TAG) << "GLAD 加载成功.";
        
        InputSystem().Init(engine->window);
        LogS(TAG) << "InputSystem 加载成功.";
        return true;
    }

    Engine *Engine::GetIns() {
        static std::unique_ptr<Engine> instance = std::unique_ptr<Engine>(new Engine());
        return instance.get();
    }
    
    void Engine::Loop() {
        PresetsLoader::LoadAll();
        Ready();
        double DeltaTime = 0;
        while (!glfwWindowShouldClose(window)) {
            EventBus().EnginePreProcess.Invoke(DeltaTime);
            DeltaTime = Process(DeltaTime);
            EventBus().EnginePostProcess.Invoke(DeltaTime);
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        Destroy();
    }

    void Engine::Exit() const {
        glfwSetWindowShouldClose(window, true);
    }

    void Engine::Ready() {
        // 背面剔除
        glEnable(GL_CULL_FACE);

        // 深度测试
        glEnable(GL_DEPTH_TEST);

        // 打印最大Uniform数量
        GLint maxUniformLocations;
        glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &maxUniformLocations);
        LogI(TAG) << "当前设备最大Uniform数量: " << maxUniformLocations;

        // GBuffer
        // int width, height;
        // glfwGetFramebufferSize(window, &width, &height);
        // GBuffer::Init(window, width, height);
        EventBus().FramebufferResized += RenderUnit::GBuffer::Init;

        // 主动获取 GLFWwindow 指针
        EventBus().GetWindowPtr += [this]() { return this->window; };

        // 窗口大小改变事件
        glfwSetFramebufferSizeCallback(window, [](GLFWwindow *_window, int _width, int _height) {
            glViewport(0, 0, _width, _height);
            LogI(TAG) << "设置Viewport: " << _width << "x" << _height;
            EventBus().FramebufferResized.Invoke(_window, _width, _height);
        });

        // 主动获取窗口大小
        EventBus().GetFramebufferSize += [this]() {
            int width, height;
            glfwGetFramebufferSize(this->window, &width, &height);
            return std::make_pair(width, height);
        };

        // 订阅Camera激活事件
        EventBus().CameraActivated += [this](void *cam) {
            LogI(TAG) << "活动相机变更";
            this->CurrentCamera = static_cast<Camera *>(cam);
            this->CurrentCamera3D = dynamic_cast<Camera3D *>(this->CurrentCamera);
        };

        // 编译着色器
        ShaderManager::LoadShaderProgram();

        // 触发Event
        EventBus().EngineReady.Invoke();
    }

    double Engine::Process(const double DeltaTime) {
        // 计时开始
        const double time = glfwGetTime();
        // 设置清空颜色
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        // 清空颜色缓冲区 | 深度缓冲区
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // 重置纹理槽
        Texture::ResetTextureSlot();
        // 获得视图矩阵和透视矩阵
        glm::mat4 viewM, projectM;
        if (CurrentCamera->IsValid()) {
            viewM = CurrentCamera->GetViewMatrix();
            projectM = CurrentCamera->GetProjectionMatrix();
        } else {
            viewM = glm::mat4(1.f);
            projectM = glm::mat4(1.f);
        }
        std::vector<std::vector<RenderUnit::Base*>> RUS(static_cast<int>(RenderUnit::Type::Count));
        Light::Directional *DirectionalLight = nullptr;
        std::vector<Light::Base*> Lights;
        std::stack<Node *> stack;
        stack.push(RootNode);
        while (!stack.empty()) {
            Node *node = stack.top();
            stack.pop();
            if (!node->IsActive()) continue;
            if (node->GetChildCount() > 0)
                for (const auto child: node->GetChildren())
                    stack.push(child);

            // Behaviour
            if (const auto behaviour = node->GetBehaviour(); behaviour != nullptr)
                behaviour->Process(DeltaTime);

            // RenderUnit
            if (node->IsType(NodeType::RenderUnit3D)) {
                auto ru3d = static_cast<RenderUnit3D *>(node);
                auto ru = ru3d->GetRU();
                switch (ru->GetType()) {
                    case RenderUnit::Type::Base: RUS[static_cast<int>(RenderUnit::Type::Base)].push_back(ru); break;
                    case RenderUnit::Type::PBR: RUS[static_cast<int>(RenderUnit::Type::PBR)].push_back(ru); break;
                    case RenderUnit::Type::Deferred_PBR: RUS[static_cast<int>(RenderUnit::Type::Deferred_PBR)].push_back(ru); break;
                    default: break;
                }
            }
            // Light
            else if (node->IsType(NodeType::Light3D)) {
                auto l = static_cast<Light3D*>(node)->GetLight();
                if (l->GetType() == Light::Type::Directional) {
                    if (DirectionalLight == nullptr) DirectionalLight = static_cast<Light::Directional*>(l);
                } else Lights.push_back(l);
            }
        }
        auto camPos = CurrentCamera3D != nullptr ? CurrentCamera3D->GetPosition() : WorldZero;
        // 帧常量
        Utils::UploadFrameConstantsUBO(camPos, DirectionalLight);
        // 点光
        Light::Point::UploadSSBO(Lights);
        // 执行渲染
        RenderUnit::Base::RenderAll(RUS[static_cast<int>(RenderUnit::Type::Base)], viewM, projectM);
        RenderUnit::PBR::RenderAll(RUS[static_cast<int>(RenderUnit::Type::PBR)], viewM, projectM);
        RenderUnit::Deferred::RenderAll(RUS[static_cast<int>(RenderUnit::Type::Deferred_PBR)], viewM, projectM);

        ui->ProcessUI();
        return (glfwGetTime() - time) * 1000.0;
    }

    void Engine::Destroy() {
        EventBus().EngineDestroy.Invoke();
        glfwDestroyWindow(window);
        glfwTerminate();
        delete RootNode;
        delete ui;
        UI::Destroy();
        Mesh::Cleanup();
    }

    std::pair<int, int> Engine::GetScreenSize() const {
        int _window_width, _window_height;
        glfwGetWindowSize(window, &_window_width, &_window_height);
        return std::make_pair(_window_width, _window_height);
    }
}
