#include "ViewportPanel.h"
#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"
#include "Grid.h"
#include "MathUtils.h"
#include "../Scene.h"
#include "GameObject.h"
#include "components/TransformComponent.h"
#include "components/MeshRendererComponent.h"
#include "imgui.h"
#include <algorithm>
#include <iostream>
#include <cmath>
#include <string>

ViewportPanel::ViewportPanel() 
    : camera_(std::make_unique<Camera>())
    , grid_(std::make_unique<Grid>(20, 1.0f))
{
}

ViewportPanel::~ViewportPanel() {
    destroyFBO();
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

void ViewportPanel::handleCameraControls() {
    if (!ImGui::IsWindowHovered()) return;

    ImGuiIO& io = ImGui::GetIO();

    // Mouse drag (any button): rotate/orbit
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) ||
        ImGui::IsMouseDragging(ImGuiMouseButton_Right) ||
        ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
        float deltaX = io.MouseDelta.x * 0.005f;
        float deltaY = io.MouseDelta.y * 0.005f;
        // Invert horizontal (drag right => look left), keep vertical (drag down => look down)
        camera_->orbit(-deltaX, deltaY);
    }

    // Middle mouse drag additionally allows fine pan if holding Shift
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle) && io.KeyShift) {
        camera_->pan(io.MouseDelta.x, -io.MouseDelta.y);
    }

    // WASD movement while hovered
    float forward = 0.0f, right = 0.0f, up = 0.0f;
    if (ImGui::IsKeyDown(ImGuiKey_W)) forward += 1.0f;
    if (ImGui::IsKeyDown(ImGuiKey_S)) forward -= 1.0f;
    if (ImGui::IsKeyDown(ImGuiKey_D)) right += 1.0f;
    if (ImGui::IsKeyDown(ImGuiKey_A)) right -= 1.0f;
    if (ImGui::IsKeyDown(ImGuiKey_E)) up += 1.0f;   // Elevate
    if (ImGui::IsKeyDown(ImGuiKey_Q)) up -= 1.0f;   // Lower
    if (forward != 0.0f || right != 0.0f || up != 0.0f) {
        // Normalize diagonal speed
        float len = sqrtf(forward*forward + right*right + up*up);
        if (len > 0.0f) { forward /= len; right /= len; up /= len; }
        camera_->moveRelative(forward, right, up);
    }

    // Mouse wheel: zoom
    float wheel = io.MouseWheel;
    if (wheel != 0.0f) {
        camera_->zoom(wheel);
    }
}

void ViewportPanel::renderScene(Shader& shader, Scene* scene) {
    // Get camera matrices
    float view[16];
    float proj[16];
    float aspect = (float)texW_ / (float)texH_;
    
    camera_->getViewMatrix(view);
    camera_->getProjectionMatrix(proj, aspect);
    
    // Set up rendering state
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glViewport(0, 0, texW_, texH_);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();
    shader.setMat4("uProjection", proj);
    shader.setMat4("uView", view);

    // Render grid as an infinite-looking plane by recentering under the camera target (pan)
    // Get camera eye and derive target by subtracting orbit vector
    float eyeX, eyeY, eyeZ;
    camera_->getEyePosition(eyeX, eyeY, eyeZ);
    // We approximate the camera target as (panX, panY, panZ) but Camera doesn't expose directly;
    // instead, center the grid on eye projected onto Y=0 and snapped to spacing for stability.
    const float spacing = 1.0f;
    float gx = floorf(eyeX / spacing) * spacing;
    float gz = floorf(eyeZ / spacing) * spacing;
    float gridModel[16];
    // Large uniform scale to cover far distances visually
    MathUtils::buildModelMatrix(gx, 0.0f, gz, 0, 0, 0, 1.0f, 1.0f, 1.0f, gridModel);
    shader.setMat4("uModel", gridModel);
    grid_->render(shader);

    // Render scene objects
        auto& gameObjects = scene->getGameObjects();
        for (size_t i = 0; i < gameObjects.size(); ++i) {
            auto* go = gameObjects[i].get();
            auto* transform = go->getTransform();
            auto* meshRenderer = go->getComponent<MeshRendererComponent>();
        
            // Skip if no mesh renderer or no mesh
            if (!meshRenderer || !meshRenderer->mesh) continue;
        
        // Build model matrix from transform
        float model[16];
        MathUtils::buildModelMatrix(
                transform->x, transform->y, transform->z,
                transform->rotX * MathUtils::DEG_TO_RAD,
                transform->rotY * MathUtils::DEG_TO_RAD,
                transform->rotZ * MathUtils::DEG_TO_RAD,
                transform->scaleX, transform->scaleY, transform->scaleZ,
            model
        );
        
        shader.setMat4("uModel", model);
        
        // Color based on selection
        if (scene->getSelectedIndex() == (int)i) {
            shader.setVec4("uColor", 1.0f, 1.0f, 0.3f, 1.0f); // Yellow for selected
        } else {
            shader.setVec4("uColor", 0.8f, 0.5f, 0.2f, 1.0f); // Orange for normal
        }
        
            meshRenderer->mesh->draw();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ViewportPanel::handleSelection(Scene* scene, bool isImageHovered) {
    if (!isImageHovered) return;

    printf("[Selection] Click detected on viewport image\n");

    // Get mouse position relative to the viewport image
    ImVec2 mousePos = ImGui::GetMousePos();
    ImVec2 imageMin = ImGui::GetItemRectMin();
    ImVec2 imageMax = ImGui::GetItemRectMax();

    // Convert to normalized device coordinates [-1, 1]
    float ndcX = ((mousePos.x - imageMin.x) / (imageMax.x - imageMin.x)) * 2.0f - 1.0f;
    float ndcY = 1.0f - ((mousePos.y - imageMin.y) / (imageMax.y - imageMin.y)) * 2.0f;

    // Get camera matrices
    float view[16], proj[16];
    float aspect = (float)texW_ / (float)texH_;
    camera_->getViewMatrix(view);
    camera_->getProjectionMatrix(proj, aspect);

    // Ray origin: camera position
    float rayOriginX, rayOriginY, rayOriginZ;
    camera_->getEyePosition(rayOriginX, rayOriginY, rayOriginZ);
    printf("[Selection] Ray origin: %f %f %f\n", rayOriginX, rayOriginY, rayOriginZ);

    // Ray direction: unproject NDC to world
    // We'll use a simple approach: project NDC to near plane, then transform by inverse view
    float near = 0.1f;
    float rayClip[4] = { ndcX, ndcY, -1.0f, 1.0f };
    float invProj[16], invView[16];
    MathUtils::invertMatrix(proj, invProj);
    MathUtils::invertMatrix(view, invView);
    float rayEye[4];
    for (int i = 0; i < 4; ++i) {
        rayEye[i] = invProj[i*4+0] * rayClip[0] + invProj[i*4+1] * rayClip[1] +
                    invProj[i*4+2] * rayClip[2] + invProj[i*4+3] * rayClip[3];
    }
    rayEye[2] = -1.0f; rayEye[3] = 0.0f;
    float rayWorld[4];
    for (int i = 0; i < 4; ++i) {
        rayWorld[i] = invView[i*4+0] * rayEye[0] + invView[i*4+1] * rayEye[1] +
                      invView[i*4+2] * rayEye[2] + invView[i*4+3] * rayEye[3];
    }
    float rayDirX = rayWorld[0];
    float rayDirY = rayWorld[1];
    float rayDirZ = rayWorld[2];
    float rayLen = sqrtf(rayDirX*rayDirX + rayDirY*rayDirY + rayDirZ*rayDirZ);
    rayDirX /= rayLen; rayDirY /= rayLen; rayDirZ /= rayLen;
    printf("[Selection] Ray dir: %f %f %f\n", rayDirX, rayDirY, rayDirZ);

    // Ray-triangle intersection for each mesh
        auto& gameObjects = scene->getGameObjects();
    int closestIndex = -1;
    float closestDist = 1e30f;
        for (size_t i = 0; i < gameObjects.size(); ++i) {
            auto* go = gameObjects[i].get();
            auto* transform = go->getTransform();
            auto* meshRenderer = go->getComponent<MeshRendererComponent>();
        
            // Skip if no mesh renderer or no mesh
            if (!meshRenderer || !meshRenderer->mesh) continue;
        
            const auto& verts = meshRenderer->mesh->getVertices();
        float model[16];
        MathUtils::buildModelMatrix(
                transform->x, transform->y, transform->z,
                transform->rotX * MathUtils::DEG_TO_RAD,
                transform->rotY * MathUtils::DEG_TO_RAD,
                transform->rotZ * MathUtils::DEG_TO_RAD,
                transform->scaleX, transform->scaleY, transform->scaleZ,
            model
        );
        // Test all triangles
        for (size_t j = 0; j + 2 < verts.size(); j += 3) {
            // Transform vertices to world space
            float v0x = model[0]*verts[j].x + model[4]*verts[j].y + model[8]*verts[j].z + model[12];
            float v0y = model[1]*verts[j].x + model[5]*verts[j].y + model[9]*verts[j].z + model[13];
            float v0z = model[2]*verts[j].x + model[6]*verts[j].y + model[10]*verts[j].z + model[14];
            float v1x = model[0]*verts[j+1].x + model[4]*verts[j+1].y + model[8]*verts[j+1].z + model[12];
            float v1y = model[1]*verts[j+1].x + model[5]*verts[j+1].y + model[9]*verts[j+1].z + model[13];
            float v1z = model[2]*verts[j+1].x + model[6]*verts[j+1].y + model[10]*verts[j+1].z + model[14];
            float v2x = model[0]*verts[j+2].x + model[4]*verts[j+2].y + model[8]*verts[j+2].z + model[12];
            float v2y = model[1]*verts[j+2].x + model[5]*verts[j+2].y + model[9]*verts[j+2].z + model[13];
            float v2z = model[2]*verts[j+2].x + model[6]*verts[j+2].y + model[10]*verts[j+2].z + model[14];
            float t = 0.0f;
            if (MathUtils::rayTriangleIntersect(
                rayOriginX, rayOriginY, rayOriginZ,
                rayDirX, rayDirY, rayDirZ,
                v0x, v0y, v0z,
                v1x, v1y, v1z,
                v2x, v2y, v2z,
                t)) {
                printf("[Selection] Hit triangle: mesh %zu tri %zu t=%f\n", i, j/3, t);
                if (t < closestDist && t > 0.0f) {
                    closestDist = t;
                    closestIndex = (int)i;
                }
            }
        }
    }
    printf("[Selection] Closest index: %d\n", closestIndex);
    if (closestIndex == -1) {
        printf("[Selection] Unselecting all objects\n");
    }
    scene->setSelectedIndex(closestIndex);
}

void ViewportPanel::handleDragDrop(Scene* scene) {
    if (!ImGui::BeginDragDropTarget()) return;
    
    const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_MESH");
    if (payload && payload->Data) {
        std::string meshName((const char*)payload->Data);
        
        // Spawn at origin, slightly above ground plane
        // TODO: Implement ray-plane intersection for proper 3D placement
        if (meshName == "Sphere") {
            scene->addSphere(1.0f, 48, 0.0f, 1.0f, 0.0f, "Sphere");
        } else if (meshName == "Pyramid") {
            scene->addPyramid(1.0f, 0.0f, 1.0f, 0.0f, "Pyramid");
        } else if (meshName == "Cube") {
            scene->addCube(1.0f, 0.0f, 1.0f, 0.0f, "Cube");
        }
    }
    
    ImGui::EndDragDropTarget();
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

    // Handle camera input
    handleCameraControls();

    // Render the scene to FBO
    renderScene(shader, scene);

    // Display the rendered texture
    ImVec2 imgSize = ImVec2((float)texW_, (float)texH_);
    ImGui::Image((ImTextureID)(intptr_t)colorTex_, imgSize, ImVec2(0, 1), ImVec2(1, 0));

    // Handle object selection (use image item, not window)
    bool isImageHovered = ImGui::IsItemHovered();
    if (ImGui::IsItemClicked()) {
        handleSelection(scene, isImageHovered);
    }

    // Handle drag-drop for spawning objects
    handleDragDrop(scene);

    ImGui::EndChild();
}