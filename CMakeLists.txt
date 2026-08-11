cmake_minimum_required(VERSION 3.20)
project(PG3DTrainer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================
# Dependencies via FetchContent
# ============================================
include(FetchContent)

# MinHook — function hooking library
FetchContent_Declare(
    minhook
    GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(minhook)

# ImGui — immediate mode GUI
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.8-docking
)
FetchContent_MakeAvailable(imgui)

# nlohmann/json — JSON config
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)
FetchContent_MakeAvailable(json)

# ============================================
# ImGui library (static, with D3D11 + Win32 backends)
# ============================================
add_library(imgui_lib STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
)
target_include_directories(imgui_lib PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui_lib PUBLIC d3d11 dxgi)

# ============================================
# Trainer DLL (injected into game process)
# ============================================
add_library(trainer SHARED
    src/dllmain.cpp
)
target_include_directories(trainer PRIVATE
    src
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(trainer PRIVATE
    minhook
    imgui_lib
    nlohmann_json::nlohmann_json
    d3d11
    dxgi
)
# Output as trainer.dll
set_target_properties(trainer PROPERTIES
    OUTPUT_NAME "trainer"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# ============================================
# Standalone Injector (console app)
# ============================================
add_executable(injector
    src/injector.cpp
)
set_target_properties(injector PROPERTIES
    OUTPUT_NAME "PG3D_Injector"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# ============================================
# Post-build: Copy DLL next to injector
# ============================================
add_custom_command(TARGET trainer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:trainer>
        "${CMAKE_BINARY_DIR}/bin/trainer.dll"
    COMMENT "Copying trainer.dll to bin/"
)
