# CEngine

基于 OpenGL 的 3D 引擎（未完成）

### 依赖
#### Windows
- XMake / CMake
- MSVC (Build Tools for Visual Studio)

#### Linux
```shell
apt install clang-21 libc++-21-dev libc++abi-21-dev xmake libglfw3-dev libassimp-dev
```

### 运行示例
#### C++
```c++
import CEngine.Engine;
import CEngine.PresetsLoader;
import CEngine.EditorUI;
import CEngine.Node;

int main(){
    // 初始化引擎
    const auto engine = CEngine::Engine::GetIns();
    // 创建窗口
    engine->NewWindow(1920, 1080, "title");
    // 加载内置资源
    CEngine::PresetsLoader::LoadAll();
    // 设置UI
    engine->setUI(new CEngine::EditorUI());
    // 添加漫游相机
    engine->Event_Ready += [&]() {
        const auto camera = CEngine::Camera3D::Create();
        camera->SetBehaviour(CEngine::BehaviourFactory::CreateBehaviour("Fly Camera"));
        engine->getToolNode()->AddChild(camera);
        camera->Active();
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
