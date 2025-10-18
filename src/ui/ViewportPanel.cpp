#include "ViewportPanel.h"
#include "Shader.h"
#include "Mesh.h"
#include "glm_ortho.h"
#include "glm_perspective.h"
#include "Meshes.h"
#include "../Scene.h"
#include "imgui.h"
#include <algorithm>
#include <iostream>

ViewportPanel::ViewportPanel() {}
ViewportPanel::~ViewportPanel() { destroyFBO(); }

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

void ViewportPanel::render(Shader& shader, Scene* scene) {
    ImGui::BeginChild("Viewport", ImVec2(0, 0), true);
    ImVec2 avail = ImGui::GetContentRegionAvail();
    int w = std::max(1, (int)avail.x);
    int h = std::max(1, (int)avail.y);

    ensureFBO(w, h);

    if (!scene) {
        ImGui::EndChild();
        return;
    }

    auto& instances = scene->getInstances();

    // Render scene into FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glViewport(0, 0, texW_, texH_);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glDisable(GL_CULL_FACE); // Disable face culling for debugging

    float proj[16];
    float aspect = (float)texW_ / (float)texH_;
    perspective(45.0f * 3.1415926f / 180.0f, aspect, 0.01f, 100.0f, proj);

    // Camera orbit controls (simple)
    static float camDist = 5.0f;
    static float camYaw = 0.0f, camPitch = 0.0f;
    ImGui::Text("Camera: LMB drag to orbit, RMB drag to pan, scroll to zoom");
    if (ImGui::IsWindowHovered()) {
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            camYaw   += ImGui::GetIO().MouseDelta.x * 0.01f;
            camPitch += ImGui::GetIO().MouseDelta.y * 0.01f;
            camPitch = std::max(-1.5f, std::min(1.5f, camPitch));
        }
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
            // TODO: implement panning if desired
        }
        camDist -= ImGui::GetIO().MouseWheel * 0.5f;
    camDist = std::max(2.0f, std::min(20.0f, camDist));
    }
    // Camera position
    float cx = camDist * cosf(camPitch) * sinf(camYaw);
    float cy = camDist * sinf(camPitch);
    float cz = camDist * cosf(camPitch) * cosf(camYaw);
    float eye[3] = { cx, cy, cz };
    float center[3] = { 0, 0, 0 };
    float up[3] = { 0, 1, 0 };
    float view[16];
    // Build view matrix (lookAt)
    auto norm = [](float* v) { float l = sqrtf(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]); v[0]/=l; v[1]/=l; v[2]/=l; };
    float f[3] = { center[0]-eye[0], center[1]-eye[1], center[2]-eye[2] };
    norm(f);
    float s[3] = { f[1]*up[2]-f[2]*up[1], f[2]*up[0]-f[0]*up[2], f[0]*up[1]-f[1]*up[0] };
    norm(s);
    float u[3] = { s[1]*f[2]-s[2]*f[1], s[2]*f[0]-s[0]*f[2], s[0]*f[1]-s[1]*f[0] };
    view[0]=s[0]; view[1]=u[0]; view[2]=-f[0]; view[3]=0;
    view[4]=s[1]; view[5]=u[1]; view[6]=-f[1]; view[7]=0;
    view[8]=s[2]; view[9]=u[2]; view[10]=-f[2]; view[11]=0;
    view[12]=-(s[0]*eye[0]+s[1]*eye[1]+s[2]*eye[2]);
    view[13]=-(u[0]*eye[0]+u[1]*eye[1]+u[2]*eye[2]);
    view[14]=-(f[0]*eye[0]+f[1]*eye[1]+f[2]*eye[2]);
    view[15]=1;

    shader.use();
    shader.setMat4("uProjection", proj);
    shader.setMat4("uView", view);
    for (size_t i = 0; i < instances.size(); ++i) {
        const auto& inst = instances[i];
        // Build model matrix from Transform
        float tx = inst.transform.x, ty = inst.transform.y, tz = inst.transform.z;
        float sx = inst.transform.scaleX, sy = inst.transform.scaleY, sz = inst.transform.scaleZ;
        float rx = inst.transform.rotX * 0.0174532925f;
        float ry = inst.transform.rotY * 0.0174532925f;
        float rz = inst.transform.rotZ * 0.0174532925f;
        float cosx=cosf(rx), sinx=sinf(rx), cosy=cosf(ry), siny=sinf(ry), cosz=cosf(rz), sinz=sinf(rz);
        float model[16] = {
            cosy*cosz*sx, -cosy*sinz*sx, siny*sx, 0,
            (sinx*siny*cosz+cosx*sinz)*sy, (-sinx*siny*sinz+cosx*cosz)*sy, -sinx*cosy*sy, 0,
            (-cosx*siny*cosz+sinx*sinz)*sz, (cosx*siny*sinz+sinx*cosz)*sz, cosx*cosy*sz, 0,
            tx, ty, tz, 1
        };
        shader.setMat4("uModel", model);
        // Highlight selected
        if (scene->getSelectedIndex() == (int)i) {
            shader.setVec4("uColor", 1.0f, 1.0f, 0.2f, 1.0f); // yellow
        } else {
            shader.setVec4("uColor", 1.0f, 0.5f, 0.2f, 1.0f); // orange
        }
        inst.mesh->draw();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Display texture in ImGui (flip vertically)
    ImVec2 imgSize = ImVec2((float)texW_, (float)texH_);
    ImVec2 imgPos = ImGui::GetCursorScreenPos();
    ImGui::Image((ImTextureID)(intptr_t)colorTex_, imgSize, ImVec2(0,1), ImVec2(1,0));
    
    // Click to select
    if (ImGui::IsItemClicked()) {
        ImVec2 mouse = ImGui::GetIO().MousePos;
        float rx = (mouse.x - imgPos.x) / std::max(1.0f, imgSize.x);
        float ry = (mouse.y - imgPos.y) / std::max(1.0f, imgSize.y);
        ry = 1.0f - ry;
        float worldX = (rx * 2.0f - 1.0f) * aspect;
        float worldY = ry * 2.0f - 1.0f;
        // Simple selection: pick closest instance (could improve with bounding boxes)
        int closest = -1;
        float minDist = 0.3f; // threshold
        for (size_t i = 0; i < instances.size(); ++i) {
            float dx = instances[i].transform.x - worldX;
            float dy = instances[i].transform.y - worldY;
            float dist = sqrtf(dx*dx + dy*dy);
            if (dist < minDist) {
                minDist = dist;
                closest = (int)i;
            }
        }
        scene->setSelectedIndex(closest);
    }
    
    // Drop target over the viewport image
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_MESH")) {
            const char* name = (const char*)payload->Data;
            if (name) {
                // Map mouse to world coords (-aspect..aspect, -1..1)
                ImVec2 mouse = ImGui::GetIO().MousePos;
                float rx = (mouse.x - imgPos.x) / std::max(1.0f, imgSize.x);
                float ry = (mouse.y - imgPos.y) / std::max(1.0f, imgSize.y);
                ry = 1.0f - ry; // flip vertical
                float worldX = (rx * 2.0f - 1.0f) * aspect;
                float worldY = ry * 2.0f - 1.0f;
                std::string n(name);
                if (n == "Circle") scene->addCircle(1.0f, 48, worldX, worldY);
                else if (n == "Triangle") scene->addTriangle(1.0f, worldX, worldY);
                else if (n == "Square") scene->addSquare(1.0f, worldX, worldY);
            }
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::EndChild();
}
