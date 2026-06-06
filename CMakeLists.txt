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
file(GLOB CENGINE_RENDER_SOURCES "${CENGINE_ROOT}/Render/*.ixx")
file(GLOB CENGINE_IMAGE_SOURCES "${CENGINE_ROOT}/Image/*.ixx")
file(GLOB CENGINE_NODE_SOURCES "${CENGINE_ROOT}/Node/*.ixx")
file(GLOB CENGINE_NODE_BEHAVIOUR_SOURCES "${CENGINE_ROOT}/Node/Behaviour/*.ixx")
file(GLOB_RECURSE CENGINE_UI_SOURCES "${CENGINE_ROOT}/UI/*.ixx")
file(GLOB_RECURSE CENGINE_PRESETS_SOURCES "${CENGINE_ROOT}/Presets/*.ixx")

add_library(CEngine STATIC)

target_sources(CEngine
    PUBLIC
        FILE_SET CXX_MODULES FILES
            "${CENGINE_ROOT}/Engine.ixx"
            ${CENGINE_BASE_SOURCES}
            ${CENGINE_UTILS_SOURCES}
            ${CENGINE_RENDER_SOURCES}
            ${CENGINE_IMAGE_SOURCES}
            ${CENGINE_NODE_SOURCES}
            ${CENGINE_NODE_BEHAVIOUR_SOURCES}
            ${CENGINE_UI_SOURCES}
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

# ==========================================
# 导出宏/函数：供主程序调用以拷贝资产和 DLL
# ==========================================
function(target_deploy_cengine_assets TARGET_NAME)
    set(FUNC_DIR "${CMAKE_CURRENT_FUNCTION_LIST_DIR}")

    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${FUNC_DIR}/Presets/Shader"
            "$<TARGET_FILE_DIR:${TARGET_NAME}>/Shader"
        
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${FUNC_DIR}/Presets/Mesh"
            "$<TARGET_FILE_DIR:${TARGET_NAME}>/Mesh"
        
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${FUNC_DIR}/ThirdParty/imgui/misc/fonts/Roboto-Medium.ttf"
            "$<TARGET_FILE_DIR:${TARGET_NAME}>/"
        
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${FUNC_DIR}/ThirdParty/glfw3/lib/glfw3.dll"
            "$<TARGET_FILE_DIR:${TARGET_NAME}>/"
            
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${FUNC_DIR}/ThirdParty/assimp/lib/assimp-vc143-mt.dll"
            "$<TARGET_FILE_DIR:${TARGET_NAME}>/"
            
        COMMENT "Deploying CEngine runtime assets for ${TARGET_NAME}..."
    )
endfunction()