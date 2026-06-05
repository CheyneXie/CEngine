-- Header-only Library
add_includedirs("ThirdParty/include")

-- GLFW (Windows: 本地预编译 / Linux: 系统包)
if is_plat("windows") then
    add_includedirs("ThirdParty/glfw3/include")
    add_linkdirs("ThirdParty/glfw3/lib")
else
    add_requires("glfw", {system = true})
end

-- assimp (Windows: 本地预编译 / Linux: 系统包)
if is_plat("windows") then
    add_includedirs("ThirdParty/assimp/include")
    add_linkdirs("ThirdParty/assimp/lib")
else
    add_requires("assimp", {system = true})
end

-- glad
add_includedirs("ThirdParty/GLAD/include")
target("glad")
    set_kind("static")
    add_files("ThirdParty/GLAD/src/*.c")

-- imgui
add_includedirs("ThirdParty/imgui")
target("imgui")
    set_kind("static")
    add_files("ThirdParty/imgui/imgui/*.cpp")

-- CEngine
target("CEngine")
    set_kind("static")
    add_includedirs("ThirdParty/imgui")
    add_files("Engine.ixx", {public = true})
    add_files("Base/*.ixx", {public = true})
    add_files("Utils/*.ixx", {public = true})
    add_files("Render/*.ixx", {public = true})
    add_files("Image/*.ixx", {public = true})
    add_files("Node/*.ixx", {public = true})
    add_files("Node/Behaviour/*.ixx", {public = true})
    add_files("Utils/*.ixx", {public = true})
    add_files("UI/*.ixx", {public = true})
    add_files("UI/EditorUI/EditorUI.ixx", {public = true}) -- 排序有问题，主模块前置
    add_files("UI/EditorUI/*.ixx", {public = true})
    add_files("Presets/**.ixx", {public = true})

    add_deps("glad", "imgui")

    if is_plat("windows") then
        add_links("glfw3", "assimp-vc143-mt", "ole32")
    else
        add_packages("glfw", "assimp")
    end

    -- 运行时 DLL + 资产拷贝
    after_build(function (target)
        os.cp(path.join(os.scriptdir(), "Presets/Shader"), target:targetdir())
        os.cp(path.join(os.scriptdir(), "Presets/Mesh"), target:targetdir())
        os.cp(path.join(os.scriptdir(), "ThirdParty/imgui/misc/fonts/Roboto-Medium.ttf"), target:targetdir())
        if is_plat("windows") then
            os.cp(path.join(os.scriptdir(), "ThirdParty/glfw3/lib/glfw3.dll"), target:targetdir())
            os.cp(path.join(os.scriptdir(), "ThirdParty/assimp/lib/assimp-vc143-mt.dll"), target:targetdir())
        end
    end)