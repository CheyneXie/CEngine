if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

set(CENGINE_ROOT "${CMAKE_CURRENT_LIST_DIR}")
set(THIRDPARTY_DIR "${CENGINE_ROOT}/ThirdParty")

# ==========================================
# 解析 std.ixx
# ==========================================
if(NOT TARGET std_module)
    cmake_path(GET CMAKE_CXX_COMPILER PARENT_PATH MSVC_ARCH_DIR)
    cmake_path(GET MSVC_ARCH_DIR PARENT_PATH MSVC_HOST_DIR)
    cmake_path(GET MSVC_HOST_DIR PARENT_PATH MSVC_BIN_DIR)
    cmake_path(GET MSVC_BIN_DIR PARENT_PATH MSVC_TOOL_DIR)

    set(STD_IXX_PATH "${MSVC_TOOL_DIR}/modules/std.ixx")

    if(EXISTS "${STD_IXX_PATH}")
        add_library(std_module STATIC)
        target_sources(std_module PUBLIC FILE_SET CXX_MODULES BASE_DIRS "${MSVC_TOOL_DIR}/modules" FILES "${STD_IXX_PATH}")
    else()
        message(FATAL_ERROR "Could not find MSVC std.ixx at ${STD_IXX_PATH}")
    endif()
endif()

# ==========================================
# 第三方库
# ==========================================
if(NOT TARGET glad)
    file(GLOB GLAD_SOURCES "${THIRDPARTY_DIR}/GLAD/src/*.c")
    add_library(glad STATIC ${GLAD_SOURCES})
    target_include_directories(glad PUBLIC "${THIRDPARTY_DIR}/GLAD/include")
endif()

if(NOT TARGET imgui)
    file(GLOB IMGUI_SOURCES "${THIRDPARTY_DIR}/imgui/imgui/*.cpp")
    add_library(imgui STATIC ${IMGUI_SOURCES})
    target_include_directories(imgui PUBLIC "${THIRDPARTY_DIR}/imgui" "${THIRDPARTY_DIR}/glfw3/include")
    target_link_libraries(imgui PRIVATE glad)
endif()

# ==========================================
# CEngine 核心引擎库
# ==========================================
file(GLOB CENGINE_BASE_SOURCES "${CENGINE_ROOT}/Base/*.ixx")
file(GLOB CENGINE_UTILS_SOURCES "${CENGINE_ROOT}/Utils/*.ixx")
file(GLOB CENGINE_IMAGE_SOURCES "${CENGINE_ROOT}/Image/*.ixx")
file(GLOB_RECURSE CENGINE_NODE_SOURCES "${CENGINE_ROOT}/Node/*.ixx")
file(GLOB_RECURSE CENGINE_RENDER_SOURCES "${CENGINE_ROOT}/Render/*.ixx")
file(GLOB_RECURSE CENGINE_UI_SOURCES "${CENGINE_ROOT}/UI/*.ixx")
file(GLOB_RECURSE CENGINE_SUBSYSTEM_SOURCES "${CENGINE_ROOT}/Subsystem/*.ixx")
file(GLOB_RECURSE CENGINE_PRESETS_SOURCES "${CENGINE_ROOT}/Presets/*.ixx")

add_library(CEngine STATIC)

target_sources(CEngine
    PUBLIC
        FILE_SET CXX_MODULES FILES
            "${CENGINE_ROOT}/Engine.ixx"
            ${CENGINE_BASE_SOURCES}
            ${CENGINE_UTILS_SOURCES}
            ${CENGINE_UI_SOURCES}
            ${CENGINE_RENDER_SOURCES}
            ${CENGINE_IMAGE_SOURCES}
            ${CENGINE_NODE_SOURCES}
            ${CENGINE_SUBSYSTEM_SOURCES}
            ${CENGINE_PRESETS_SOURCES}
)

target_include_directories(CEngine PUBLIC
    "${THIRDPARTY_DIR}/include"
    "${THIRDPARTY_DIR}/glfw3/include"
    "${THIRDPARTY_DIR}/assimp/include"
    "${THIRDPARTY_DIR}/imgui"
)

target_link_directories(CEngine PUBLIC
    "${THIRDPARTY_DIR}/glfw3/lib"
    "${THIRDPARTY_DIR}/assimp/lib"
)

target_link_libraries(CEngine PUBLIC
    glad
    imgui
    glfw3
    assimp-vc143-mt
    ole32
    std_module
)

# ==============================================================================
# 动态生成一个辅助脚本，用于执行 xxd 并注入 #pragma once
# ==============================================================================
set(EMBED_SCRIPT "${CMAKE_BINARY_DIR}/GenerateEmbed.cmake")
file(WRITE "${EMBED_SCRIPT}" "
    # 执行 xxd，输出到临时文件 (内部路径加上双引号防空格)
    execute_process(
        COMMAND xxd -i -name \${VAR_NAME} \"\${SRC}\" \"\${DST}.tmp\"
        RESULT_VARIABLE RES
    )
    if(NOT RES EQUAL 0)
        message(FATAL_ERROR \"xxd failed on \${SRC}\")
    endif()
    
    # 读取临时文件内容，拼接 #pragma once，写入最终文件并删除临时文件
    file(READ \"\${DST}.tmp\" CONTENT)
    file(WRITE \"\${DST}\" \"#pragma once\\n\\n\${CONTENT}\")
    file(REMOVE \"\${DST}.tmp\")
")

# ==============================================================================
# 收集文件并建立构建规则
# ==============================================================================
set(EMBED_OUTDIR "${CMAKE_BINARY_DIR}/embed_headers")

# 递归扫描这三个目录下的所有文件
file(GLOB_RECURSE EMBED_SOURCES
    "Presets/Mesh/*"
    "Presets/Shader/*"
    "ThirdParty/Fonts/*"
)

set(GENERATED_HEADERS "")

foreach(SRC IN LISTS EMBED_SOURCES)
    # 排除 .h 文件
    if(SRC MATCHES "\\.h$")
        continue()
    endif()

    # 计算相对路径，例如 Presets/Shader/Base.frag
    file(RELATIVE_PATH REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}" "${SRC}")
    get_filename_component(REL_DIR "${REL_PATH}" DIRECTORY)
    get_filename_component(FILENAME "${SRC}" NAME)

    # 最终输出路径
    set(TARGET_FILE "${EMBED_OUTDIR}/${REL_DIR}/${FILENAME}.h")

    # 自动生成 C 变量名 (如 cube.obj -> cube_obj)
    string(MAKE_C_IDENTIFIER "${FILENAME}" VAR_NAME)

    # 注册增量生成命令（只有 SRC 更新或 TARGET_FILE 丢失时才会触发）
    add_custom_command(
        OUTPUT "${TARGET_FILE}"
        COMMAND ${CMAKE_COMMAND} 
            -DSRC=${SRC} 
            -DDST=${TARGET_FILE} 
            -DVAR_NAME=${VAR_NAME} 
            -P ${EMBED_SCRIPT}
        DEPENDS "${SRC}"
        COMMENT "[Embed] converting ${REL_DIR}/${FILENAME} -> ${FILENAME}.h"
        VERBATIM
    )

    list(APPEND GENERATED_HEADERS "${TARGET_FILE}")
endforeach()

# 创建一个专用的假靶标来承载所有的生成命令
add_custom_target(CEngine_Embed DEPENDS ${GENERATED_HEADERS})

add_dependencies(CEngine CEngine_Embed)

target_include_directories(CEngine PUBLIC "${EMBED_OUTDIR}")

# ==========================================
# 导出宏/函数：供主程序调用以拷贝 DLL
# ==========================================
function(target_deploy_cengine_assets TARGET_NAME)
    set(FUNC_DIR "${CMAKE_CURRENT_FUNCTION_LIST_DIR}")

    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${FUNC_DIR}/ThirdParty/glfw3/lib/glfw3.dll"
            "$<TARGET_FILE_DIR:${TARGET_NAME}>/"
            
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${FUNC_DIR}/ThirdParty/assimp/lib/assimp-vc143-mt.dll"
            "$<TARGET_FILE_DIR:${TARGET_NAME}>/"
            
        COMMENT "Deploying CEngine runtime assets for ${TARGET_NAME}..."
    )
endfunction()