#include "Application.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <algorithm>

#include "Shader.h"
#include "Mesh.h"
#include "glm_ortho.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "ui/InspectorPanel.h"
#include "ui/ViewportPanel.h"

Application::Application() {}

Application::~Application() { shutdown(); }

bool Application::init() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    window_ = glfwCreateWindow(800, 600, "My Game Engine", nullptr, nullptr);
    if (!window_) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window_);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return false;
    }

    // ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    // Create shader
    shader_ = new Shader("../shaders/vertex.glsl", "../shaders//solid/fragment_solid.glsl");

    // Create meshes
    std::vector<Vertex> triangleVertices = {
        { 0.0f,  0.5f, 0.0f},
        {-0.5f, -0.5f, 0.0f},
        { 0.5f, -0.5f, 0.0f}
    };
    meshes_.push_back(new Mesh(triangleVertices));

    float aspect = 800.0f / 600.0f;
    std::vector<Vertex> squareVertices = {
        {-0.5f * aspect,  0.5f, 0.0f},
        {-0.5f * aspect, -0.5f, 0.0f},
        { 0.5f * aspect, -0.5f, 0.0f},
        { 0.5f * aspect, -0.5f, 0.0f},
        { 0.5f * aspect,  0.5f, 0.0f},
        {-0.5f * aspect,  0.5f, 0.0f}
    };
    meshes_.push_back(new Mesh(squareVertices));

    inspector_ = new InspectorPanel();
    viewport_ = new ViewportPanel();
    return true;
}

void Application::shutdown() {
    for (auto* m : meshes_) delete m;
    meshes_.clear();
    delete shader_; shader_ = nullptr;
    delete inspector_; inspector_ = nullptr;
    delete viewport_; viewport_ = nullptr;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (window_) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
    glfwTerminate();
}

void Application::frame() {
    int winW, winH;
    glfwGetFramebufferSize(window_, &winW, &winH);

    // Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Docked window with splitter
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)winW, (float)winH));
    ImGui::Begin("Engine", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    static float inspectorWidth = 300.0f;
    float minInspector = 150.0f;
    float maxInspector = (float)winW - 150.0f;
    inspectorWidth = std::max(minInspector, std::min(inspectorWidth, maxInspector));

    ImGui::BeginChild("InspectorContainer", ImVec2(inspectorWidth, (float)winH), true);
    inspector_->render(meshes_);
    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::InvisibleButton("Splitter", ImVec2(5.0f, (float)winH));
    if (ImGui::IsItemActive()) inspectorWidth += ImGui::GetIO().MouseDelta.x;
    ImGui::SameLine();

    ImGui::BeginChild("ViewportContainer", ImVec2(0, (float)winH), true);
    viewport_->render(*shader_, meshes_);
    ImGui::EndChild();

    ImGui::End();

    // Clear default and render ImGui
    glViewport(0, 0, winW, winH);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int Application::run() {
    if (!init()) return -1;
    while (!glfwWindowShouldClose(window_)) {
        frame();
        glfwSwapBuffers(window_);
        glfwPollEvents();
    }
    shutdown();
    return 0;
}
