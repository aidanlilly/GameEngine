# Game Engine

A minimal OpenGL playground on Windows using CMake, GLFW, and GLEW. Renders simple geometry with GLSL shaders, structured for growth (Shader and Mesh classes).

## Features
- CMake-based build
- GLFW + GLEW (static) + OpenGL
- Basic Shader and Mesh classes
- Two-triangle demo using uniforms (color + offset)

## Prerequisites
- CMake 3.10+
- MinGW (MSYS2 UCRT64 is fine) or your preferred generator
- GLFW binaries available locally
- GLEW 2.1.0 available locally

Update the include/library paths in `CMakeLists.txt` to match your machine:
- `C:/Users/aidan/glfw-3.4.bin.WIN64/...`
- `C:/Users/aidan/glew-2.1.0/...`

## Build & Run
From the repo root:

```powershell
mkdir build; cd build
cmake ..
cmake --build .
./MyGameEngine.exe
```

Or use the VS Code task "Build and Run Game Engine".

## Project Structure
```
GameEngine/
├─ include/
│  ├─ Mesh.h
│  └─ Shader.h
├─ shaders/
│  ├─ fragment.glsl
│  └─ vertex.glsl
├─ src/
│  ├─ main.cpp
│  ├─ Mesh.cpp
│  └─ Shader.cpp
├─ CMakeLists.txt
├─ .gitignore
└─ README.md
```

## Next Steps
- Add indices (EBO) and normals/UVs to Mesh
- Add a Texture class and sample textures in the fragment shader
- Integrate Assimp to load FBX/OBJ models
- Add a Camera + MVP matrices (use GLM)
