#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "Shader.h"
#include "Mesh.h"

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Create window
    GLFWwindow* window = glfwCreateWindow(800, 600, "My Game Engine", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    // Create shader
    Shader shader("../shaders/vertex.glsl", "../shaders/fragment.glsl");

    // Create triangle mesh
    std::vector<Vertex> triangleVertices = {
        { 0.0f,  0.5f, 0.0f},  // Top
        {-0.5f, -0.5f, 0.0f},  // Bottom left
        { 0.5f, -0.5f, 0.0f}   // Bottom right
    };
    Mesh triangle(triangleVertices);

    // Game loop
    while (!glfwWindowShouldClose(window)) {
        // Clear screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render two triangles using the same mesh with different uniforms
        shader.use();
        // Left triangle (orange)
        shader.setVec2("uOffset", -0.4f, 0.0f);
        shader.setVec4("uColor", 1.0f, 0.5f, 0.2f, 1.0f);
        triangle.draw();

        // Right triangle (blue)
        shader.setVec2("uOffset", 0.4f, 0.0f);
        shader.setVec4("uColor", 0.2f, 0.5f, 1.0f, 1.0f);
        triangle.draw();

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup (RAII handles shader and mesh cleanup automatically)
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
