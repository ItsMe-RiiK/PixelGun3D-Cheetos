cmake_minimum_required(VERSION 3.20)
project(PG3DTrainer VERSION 0.0.3 LANGUAGES CXX C RC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages using vcpkg
find_package(minhook CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# ============================================
# Trainer DLL (injected into game process)
# ============================================
add_library(trainer SHARED
    src/core/dllmain.cpp
    src/utils/config.cpp
    src/utils/il2cpp.cpp
    src/utils/hooks.cpp
    src/features/combat.cpp
    src/features/playermod.cpp
    src/features/visual.cpp
    src/features/weaponmod.cpp
    src/ui/menu.cpp
)
target_include_directories(trainer PRIVATE src)

# Link vcpkg libraries
target_link_libraries(trainer PRIVATE
    minhook::minhook
    imgui::imgui
    nlohmann_json::nlohmann_json
    d3d11
    dxgi
    d3dcompiler
    dwmapi
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
    src/core/injector.cpp
    resources/injector.rc
)
set_target_properties(injector PROPERTIES
    OUTPUT_NAME "PG3D_Injector"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
if(MINGW)
    target_link_options(injector PRIVATE -static)
    target_link_options(trainer PRIVATE -static)
endif()

# ============================================
# Post-build: Copy DLL next to injector
# ============================================
add_custom_command(TARGET trainer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:trainer>
        "${CMAKE_BINARY_DIR}/bin/trainer.dll"
    COMMENT "Copying trainer.dll to bin/"
)
