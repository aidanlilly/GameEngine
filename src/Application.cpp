#include "Application.h"

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
#include "ui/ProjectPanel.h"
#include "ui/PropertiesPanel.h"
#include "Scene.h"

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

    // Create scene with its meshes
    scene_ = new Scene();

    inspector_ = new InspectorPanel();
    viewport_ = new ViewportPanel();
    project_ = new ProjectPanel();
    properties_ = new PropertiesPanel();
    project_->setProjectPath("GameProject");
    return true;
}

void Application::shutdown() {
    delete scene_; scene_ = nullptr;
    delete shader_; shader_ = nullptr;
    delete inspector_; inspector_ = nullptr;
    delete viewport_; viewport_ = nullptr;
    delete project_; project_ = nullptr;
    delete properties_; properties_ = nullptr;

    // Shutdown ImGui backends before destroying context
    if (window_) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    if (window_) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
    glfwTerminate();
}

void Application::frame() {
    int winW, winH;
    glfwGetFramebufferSize(window_, &winW, &winH);

    // Handle keyboard input (Delete key)
    if (ImGui::IsKeyPressed(ImGuiKey_Delete)) {
        scene_->deleteSelected();
    }

    // Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Fullscreen window with panels
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)winW, (float)winH));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::Begin("Engine", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    static float inspectorWidth = 300.0f;
    static float propertiesWidth = 250.0f;
    static float projectHeight = 200.0f;
    const float splitterSize = 2.0f;
    const float splitterHitSize = 8.0f; // Larger hit area for dragging
    float minInspector = 150.0f;
    float maxInspector = (float)winW - 150.0f - splitterHitSize;
    float minProject = 100.0f;
    float maxProject = (float)winH - 150.0f - splitterHitSize;
    inspectorWidth = std::max(minInspector, std::min(inspectorWidth, maxInspector));
    projectHeight = std::max(minProject, std::min(projectHeight, maxProject));

    float topPanelHeight = (float)winH - projectHeight - splitterHitSize;

    // Top section: Inspector + Viewport
    ImGui::BeginChild("InspectorContainer", ImVec2(inspectorWidth, topPanelHeight), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    inspector_->render(scene_);
    ImGui::EndChild();

    ImGui::SameLine(0, 0);
    
    // Horizontal splitter bar (wider hit area, thin visual)
    ImVec2 splitterPos = ImGui::GetCursorScreenPos();
    float offsetX = (splitterHitSize - splitterSize) * 0.5f; // Center the visual bar in hit area
    ImGui::InvisibleButton("HSplitter", ImVec2(splitterHitSize, topPanelHeight));
    if (ImGui::IsItemActive()) {
        inspectorWidth += ImGui::GetIO().MouseDelta.x;
    }
    if (ImGui::IsItemHovered() || ImGui::IsItemActive()) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
    }
    // Draw thin gray bar centered in the hit area
    ImGui::GetWindowDrawList()->AddRectFilled(
        ImVec2(splitterPos.x + offsetX, splitterPos.y), 
        ImVec2(splitterPos.x + offsetX + splitterSize, splitterPos.y + topPanelHeight), 
        IM_COL32(60, 60, 60, 255)
    );
    
    ImGui::SameLine(0, 0);

    // Viewport (middle, flexible width)
    float viewportWidth = (float)winW - inspectorWidth - propertiesWidth - splitterHitSize * 3;
    ImGui::BeginChild("ViewportContainer", ImVec2(viewportWidth, topPanelHeight), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    viewport_->render(*shader_, scene_);
    ImGui::EndChild();

    // Properties panel (right side, always visible)
    ImGui::SameLine(0, 0);
    // Horizontal splitter for properties
    ImVec2 propSplitterPos = ImGui::GetCursorScreenPos();
    float propOffsetX = (splitterHitSize - splitterSize) * 0.5f;
    ImGui::InvisibleButton("HSplitter2", ImVec2(splitterHitSize, topPanelHeight));
    if (ImGui::IsItemActive()) {
        propertiesWidth -= ImGui::GetIO().MouseDelta.x;
        propertiesWidth = std::max(150.0f, std::min(propertiesWidth, 500.0f));
    }
    if (ImGui::IsItemHovered() || ImGui::IsItemActive()) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
    }
    ImGui::GetWindowDrawList()->AddRectFilled(
        ImVec2(propSplitterPos.x + propOffsetX, propSplitterPos.y), 
        ImVec2(propSplitterPos.x + propOffsetX + splitterSize, propSplitterPos.y + topPanelHeight), 
        IM_COL32(60, 60, 60, 255)
    );
    ImGui::SameLine(0, 0);
    ImGui::BeginChild("PropertiesContainer", ImVec2(propertiesWidth, topPanelHeight), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    properties_->render(scene_);
    ImGui::EndChild();

    // Vertical splitter bar (wider hit area, thin visual)
    ImVec2 vSplitterPos = ImGui::GetCursorScreenPos();
    float offsetY = (splitterHitSize - splitterSize) * 0.5f; // Center the visual bar in hit area
    ImGui::InvisibleButton("VSplitter", ImVec2((float)winW, splitterHitSize));
    if (ImGui::IsItemActive()) {
        projectHeight -= ImGui::GetIO().MouseDelta.y;
    }
    if (ImGui::IsItemHovered() || ImGui::IsItemActive()) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
    }
    // Draw thin gray bar centered in the hit area
    ImGui::GetWindowDrawList()->AddRectFilled(
        ImVec2(vSplitterPos.x, vSplitterPos.y + offsetY), 
        ImVec2(vSplitterPos.x + (float)winW, vSplitterPos.y + offsetY + splitterSize), 
        IM_COL32(60, 60, 60, 255)
    );

    // Bottom section: Project panel
    ImGui::BeginChild("BottomSection", ImVec2((float)winW, projectHeight), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    project_->render();
    ImGui::EndChild();

    ImGui::End();
    ImGui::PopStyleVar(2);

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
