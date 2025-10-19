#include "ViewportPanel.h"
#include "Shader.h"
#include "Mesh.h"
#include "glm_perspective.h"
#include "Meshes.h"
#include "../Scene.h"
#include "imgui.h"
#include <algorithm>
#include <iostream>
#include <cmath>

ViewportPanel::ViewportPanel() {
    createGridMesh();
}

ViewportPanel::~ViewportPanel() {
    destroyFBO();
    destroyGridMesh();
}

void ViewportPanel::destroyFBO() {
    if (fbo_) glDeleteFramebuffers(1, &fbo_);
    if (colorTex_) glDeleteTextures(1, &colorTex_);
    if (depthRbo_) glDeleteRenderbuffers(1, &depthRbo_);
    fbo_ = 0; colorTex_ = 0; depthRbo_ = 0; texW_ = texH_ = 0;
}

void ViewportPanel::ensureFBO(int w, int h) {
    w = std::max(1, w);
    h = std::max(1, h);
    if (fbo_ && w == texW_ && h == texH_) return;

    destroyFBO();

    texW_ = w; texH_ = h;
    glGenFramebuffers(1, &fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

    glGenTextures(1, &colorTex_);
    glBindTexture(GL_TEXTURE_2D, colorTex_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW_, texH_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex_, 0);

    glGenRenderbuffers(1, &depthRbo_);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRbo_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, texW_, texH_);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthRbo_);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Viewport FBO incomplete" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ViewportPanel::createGridMesh() {
    // Create a grid on the XZ plane (Y=0)
    const int gridSize = 20;
    const float gridSpacing = 1.0f;
    std::vector<float> vertices;

    // Lines parallel to X axis
    for (int z = -gridSize; z <= gridSize; ++z) {
        vertices.push_back(-gridSize * gridSpacing);
        vertices.push_back(0.0f);
        vertices.push_back(z * gridSpacing);
        
        vertices.push_back(gridSize * gridSpacing);
        vertices.push_back(0.0f);
        vertices.push_back(z * gridSpacing);
    }

    // Lines parallel to Z axis
    for (int x = -gridSize; x <= gridSize; ++x) {
        vertices.push_back(x * gridSpacing);
        vertices.push_back(0.0f);
        vertices.push_back(-gridSize * gridSpacing);
        
        vertices.push_back(x * gridSpacing);
        vertices.push_back(0.0f);
        vertices.push_back(gridSize * gridSpacing);
    }

    gridVertexCount_ = vertices.size() / 3;

    glGenVertexArrays(1, &gridVAO_);
    glGenBuffers(1, &gridVBO_);

    glBindVertexArray(gridVAO_);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO_);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ViewportPanel::destroyGridMesh() {
    if (gridVAO_) glDeleteVertexArrays(1, &gridVAO_);
    if (gridVBO_) glDeleteBuffers(1, &gridVBO_);
    gridVAO_ = 0;
    gridVBO_ = 0;
    gridVertexCount_ = 0;
}

void ViewportPanel::renderGrid(Shader& shader) {
    // Render grid as lines
    float identity[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    shader.setMat4("uModel", identity);
    shader.setVec4("uColor", 0.3f, 0.3f, 0.3f, 1.0f); // Gray grid
    
    glBindVertexArray(gridVAO_);
    glDrawArrays(GL_LINES, 0, gridVertexCount_);
    glBindVertexArray(0);
}

void ViewportPanel::render(Shader& shader, Scene* scene) {
    ImGui::BeginChild("Viewport", ImVec2(0, 0), true);
    
    if (!scene) {
        ImGui::EndChild();
        return;
    }

    ImVec2 avail = ImGui::GetContentRegionAvail();
    int w = std::max(1, (int)avail.x);
    int h = std::max(1, (int)avail.y);
    ensureFBO(w, h);

    // Camera controls
    bool isHovered = ImGui::IsWindowHovered();
    if (isHovered) {
        // Left mouse button: orbit camera
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            camYaw_ += ImGui::GetIO().MouseDelta.x * 0.005f;
            camPitch_ -= ImGui::GetIO().MouseDelta.y * 0.005f;
            // Clamp pitch to avoid gimbal lock
            camPitch_ = std::max(-1.5f, std::min(1.5f, camPitch_));
        }
        
        // Right mouse button: pan camera
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
            float panSpeed = 0.01f * camDistance_;
            camPanX_ += ImGui::GetIO().MouseDelta.x * panSpeed;
            camPanY_ -= ImGui::GetIO().MouseDelta.y * panSpeed;
        }
        
        // Mouse wheel: zoom
        float wheel = ImGui::GetIO().MouseWheel;
        if (wheel != 0.0f) {
            camDistance_ -= wheel * 0.5f;
            camDistance_ = std::max(1.0f, std::min(50.0f, camDistance_));
        }
    }

    // Calculate camera position (orbit around origin with pan offset)
    float cx = camDistance_ * cosf(camPitch_) * sinf(camYaw_);
    float cy = camDistance_ * sinf(camPitch_);
    float cz = camDistance_ * cosf(camPitch_) * cosf(camYaw_);
    
    float targetX = camPanX_;
    float targetY = camPanY_;
    float targetZ = 0.0f;
    
    float eyeX = cx + targetX;
    float eyeY = cy + targetY;
    float eyeZ = cz + targetZ;

    // Build view matrix (lookAt)
    float fx = targetX - eyeX;
    float fy = targetY - eyeY;
    float fz = targetZ - eyeZ;
    float flen = sqrtf(fx*fx + fy*fy + fz*fz);
    fx /= flen; fy /= flen; fz /= flen;
    
    float upX = 0.0f, upY = 1.0f, upZ = 0.0f;
    
    // Right = forward × up
    float sx = fy * upZ - fz * upY;
    float sy = fz * upX - fx * upZ;
    float sz = fx * upY - fy * upX;
    float slen = sqrtf(sx*sx + sy*sy + sz*sz);
    sx /= slen; sy /= slen; sz /= slen;
    
    // Up = right × forward
    float ux = sy * fz - sz * fy;
    float uy = sz * fx - sx * fz;
    float uz = sx * fy - sy * fx;
    
    float view[16] = {
        sx, ux, -fx, 0,
        sy, uy, -fy, 0,
        sz, uz, -fz, 0,
        -(sx*eyeX + sy*eyeY + sz*eyeZ),
        -(ux*eyeX + uy*eyeY + uz*eyeZ),
        -(-fx*eyeX + -fy*eyeY + -fz*eyeZ),
        1
    };

    // Build projection matrix
    float aspect = (float)texW_ / (float)texH_;
    float proj[16];
    perspective(45.0f * 3.14159265f / 180.0f, aspect, 0.1f, 1000.0f, proj);

    // Render to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glViewport(0, 0, texW_, texH_);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();
    shader.setMat4("uProjection", proj);
    shader.setMat4("uView", view);

    // Render grid
    renderGrid(shader);

    // Render scene objects
    auto& instances = scene->getInstances();
    for (size_t i = 0; i < instances.size(); ++i) {
        const auto& inst = instances[i];
        
        // Build model matrix from transform
        float tx = inst.transform.x;
        float ty = inst.transform.y;
        float tz = inst.transform.z;
        float sx = inst.transform.scaleX;
        float sy = inst.transform.scaleY;
        float sz = inst.transform.scaleZ;
        float rx = inst.transform.rotX * 0.0174532925f; // deg to rad
        float ry = inst.transform.rotY * 0.0174532925f;
        float rz = inst.transform.rotZ * 0.0174532925f;
        
        // Rotation matrices (ZYX order)
        float cosx = cosf(rx), sinx = sinf(rx);
        float cosy = cosf(ry), siny = sinf(ry);
        float cosz = cosf(rz), sinz = sinf(rz);
        
        float model[16] = {
            cosy*cosz*sx, (sinx*siny*cosz - cosx*sinz)*sx, (cosx*siny*cosz + sinx*sinz)*sx, 0,
            cosy*sinz*sy, (sinx*siny*sinz + cosx*cosz)*sy, (cosx*siny*sinz - sinx*cosz)*sy, 0,
            -siny*sz, sinx*cosy*sz, cosx*cosy*sz, 0,
            tx, ty, tz, 1
        };
        
        shader.setMat4("uModel", model);
        
        // Color based on selection
        if (scene->getSelectedIndex() == (int)i) {
            shader.setVec4("uColor", 1.0f, 1.0f, 0.3f, 1.0f); // Yellow for selected
        } else {
            shader.setVec4("uColor", 0.8f, 0.5f, 0.2f, 1.0f); // Orange for normal
        }
        
        inst.mesh->draw();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Display the rendered texture
    ImVec2 imgSize = ImVec2((float)texW_, (float)texH_);
    ImVec2 imgPos = ImGui::GetCursorScreenPos();
    ImGui::Image((ImTextureID)(intptr_t)colorTex_, imgSize, ImVec2(0, 1), ImVec2(1, 0));

    // Handle object selection (simple ray-based picking would be better, but for now use screen-space distance)
    if (ImGui::IsItemClicked() && isHovered) {
        // For now, just cycle through selection or implement proper 3D picking later
        int currentSelection = scene->getSelectedIndex();
        if (currentSelection >= 0 && currentSelection < (int)instances.size() - 1) {
            scene->setSelectedIndex(currentSelection + 1);
        } else {
            scene->setSelectedIndex(0);
        }
    }

    // Handle drag-drop for spawning objects
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_MESH")) {
            const char* name = (const char*)payload->Data;
            if (name) {
                std::string meshName(name);
                // Spawn at origin for now (proper 3D placement would need ray-plane intersection)
                if (meshName == "Circle") {
                    scene->addCircle(1.0f, 48, 0.0f, 1.0f, 0.0f);
                } else if (meshName == "Triangle") {
                    scene->addTriangle(1.0f, 0.0f, 1.0f, 0.0f);
                } else if (meshName == "Square") {
                    scene->addSquare(1.0f, 0.0f, 1.0f, 0.0f);
                }
            }
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::EndChild();
}