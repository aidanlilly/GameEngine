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
    src/Shader.cpp
    src/Mesh.cpp
)

# Create executable
add_executable(MyGameEngine ${SOURCES})

# Link libraries
target_link_libraries(MyGameEngine
    "C:/Users/aidan/glew-2.1.0/lib/Release/x64/glew32s.lib"  # static GLEW
    "C:/Users/aidan/glfw-3.4.bin.WIN64/lib-mingw-w64/libglfw3.a" # GLFW static
    -lopengl32
)

