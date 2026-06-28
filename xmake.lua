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

    -- 资源文件
    local embed_outdir = path.join(os.projectdir(), "build", ".gens", "embed_headers")
    add_includedirs(embed_outdir, {public = true})
    on_config(function (target)
        import("core.project.depend")
        local scriptdir = target:scriptdir()
        
        local sourcefiles = {}
        for _, f in ipairs(os.files(path.join(scriptdir, "Presets/Mesh/**")) or {}) do table.insert(sourcefiles, f) end
        for _, f in ipairs(os.files(path.join(scriptdir, "Presets/Shader/**")) or {}) do table.insert(sourcefiles, f) end
        for _, f in ipairs(os.files(path.join(scriptdir, "ThirdParty/Fonts/**")) or {}) do table.insert(sourcefiles, f) end

        for _, sourcefile in ipairs(sourcefiles) do
            if not sourcefile:endswith(".h") then
                local relative_dir = path.directory(path.relative(sourcefile, scriptdir))
                local filename = path.filename(sourcefile) .. ".h"
                local targetfile = path.join(embed_outdir, relative_dir, filename)
                depend.on_changed(function ()
                    os.mkdir(path.directory(targetfile))
                    local varname = path.basename(sourcefile) .. "_" .. path.extension(sourcefile):gsub("%.", "")
                    cprint("${yellow}[Embed] ${clear}converting %s -> %s/%s", path.filename(sourcefile), relative_dir, filename)
                    -- 执行xxd
                    os.vrunv("xxd", {"-i", "-name", varname, sourcefile, targetfile})
                    -- 添加 #pragma once
                    local content = io.readfile(targetfile)
                    if content then
                        io.writefile(targetfile, "#pragma once\n\n" .. content)
                    end
                end, {files = {sourcefile}, targetfile = targetfile})
            end
        end
    end)

    -- 源码
    add_includedirs("ThirdParty/imgui")
    add_files("Engine.ixx", {public = true})
    add_files("Base/*.ixx", {public = true})
    add_files("Utils/*.ixx", {public = true})
    add_files("Render/**.ixx", {public = true})
    add_files("Image/*.ixx", {public = true})
    add_files("Node/*.ixx", {public = true})
    add_files("Node/Behaviour/*.ixx", {public = true})
    add_files("Subsystem/*.ixx", {public = true})
    add_files("Utils/*.ixx", {public = true})
    add_files("UI/*.ixx", {public = true})
    add_files("UI/EditorUI/**.ixx", {public = true})
    add_files("Presets/**.ixx", {public = true})

    add_deps("glad", "imgui")

    if is_plat("windows") then
        add_links("glfw3", "assimp-vc143-mt", "ole32")
    else
        add_packages("glfw", "assimp")
    end

    -- 运行时 DLL
    after_build(function (target)
        if is_plat("windows") then
            os.cp(path.join(os.scriptdir(), "ThirdParty/glfw3/lib/glfw3.dll"), target:targetdir())
            os.cp(path.join(os.scriptdir(), "ThirdParty/assimp/lib/assimp-vc143-mt.dll"), target:targetdir())
        end
    end)