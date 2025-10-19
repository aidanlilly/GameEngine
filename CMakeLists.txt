cmake_minimum_required(VERSION 3.10)
project(GameEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    "C:/Users/aidan/glew-2.1.0/include"
    "C:/Users/aidan/glfw-3.4.bin.WIN64/include"
)

# Source files

set(SOURCES
    src/main.cpp
    src/Application.cpp
    src/Scene.cpp
    src/GameObject.cpp
    src/components/TransformComponent.cpp
    src/components/MeshRendererComponent.cpp
    src/components/LightComponent.cpp
    src/components/MaterialComponent.cpp
    src/Shader.cpp
    src/Mesh.cpp
    src/Camera.cpp
    src/Grid.cpp
    src/ui/InspectorPanel.cpp
    src/ui/ViewportPanel.cpp
    src/ui/ProjectPanel.cpp
    src/ui/PropertiesPanel.cpp

)

# ImGui sources
set(IMGUI_SOURCES
    include/imgui/imgui.cpp
    include/imgui/imgui_draw.cpp
    include/imgui/imgui_tables.cpp
    include/imgui/imgui_widgets.cpp
    include/imgui/backends/imgui_impl_opengl3.cpp
    include/imgui/backends/imgui_impl_glfw.cpp
)

# ImGui include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include/imgui
    ${PROJECT_SOURCE_DIR}/include/imgui/backends
)

# Create ImGui static library
add_library(imgui STATIC ${IMGUI_SOURCES})
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)

# Create executable
add_executable(MyGameEngine ${SOURCES})
target_compile_definitions(MyGameEngine PUBLIC GLEW_STATIC)

# Link ImGui to your engine
target_link_libraries(MyGameEngine imgui)

# Link libraries
target_link_libraries(MyGameEngine
    "C:/Users/aidan/glew-2.1.0/lib/Release/x64/glew32s.lib"  # static GLEW
    "C:/Users/aidan/glfw-3.4.bin.WIN64/lib-mingw-w64/libglfw3.a" # GLFW static
    opengl32
)

