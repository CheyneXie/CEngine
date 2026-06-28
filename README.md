# CEngine

基于 OpenGL 的 3D 引擎

### 依赖
#### Windows
- XMake / CMake
- MSVC (Build Tools for Visual Studio)
- xxd (Git Bash 套件里有)

#### Linux
```shell
apt install clang-21 libc++-21-dev libc++abi-21-dev xmake libglfw3-dev libassimp-dev xxd
```

### 运行示例
#### C++
```c++
import CEngine.Engine;
import CEngine.EditorUI;
import CEngine.Node;
import CEngine.EventBus;

int main(){
    // 初始化引擎
    CEngine::Engine::Init(1920, 1080, "title");
    const auto engine = CEngine::Engine::GetIns();
    // 设置UI
    engine->setUI(new CEngine::EditorUI());
    // Ready事件
    CEngine::EventBus().EngineReady += [engine]() {
        // 添加平行光
        engine->getRoot()->AddChild(CEngine::Light3D::Create());
        // 添加漫游相机
        const auto camera = CEngine::Camera3D::Create();
        camera->SetBehaviour(CEngine::BehaviourFactory::CreateBehaviour("Fly Camera"));
        engine->getRoot()->AddChild(camera);
        camera->Activate();
    };
    // 进入循环
    engine->Loop();
    return 0;
}
```
#### XMake
```lua
set_languages("c++23")
set_policy("build.c++.modules", true)

-- Linux + Clang
if is_plat("linux") then
    set_toolchains("clang")
    set_runtimes("c++_static")
    add_ldflags("-static-libstdc++", {force = true})
    add_syslinks("c++abi")
    -- libc++-21 std.cppm chrono.inc 中的 operator""d / operator""y 会触发报错
    add_cxflags("-Wno-reserved-user-defined-literal")
end

includes("CEngine")

target("xxx")
    set_kind("binary")
    add_files("main.cpp")
    add_deps("CEngine")
```

#### CMake
仅支持 Windows
```cmake
cmake_minimum_required(VERSION 3.28)

project(XXX C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(CEngine)
add_executable(XXX main.cpp)
target_link_libraries(XXX PRIVATE CEngine)

target_deploy_cengine_assets(XXX)
```
