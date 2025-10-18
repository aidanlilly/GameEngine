#include "ViewportPanel.h"
#include "Shader.h"
#include "Mesh.h"
#include "glm_ortho.h"
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

void ViewportPanel::render(Shader& shader, const std::vector<Mesh*>& meshes) {
    ImGui::BeginChild("Viewport", ImVec2(0, 0), true);
    ImVec2 avail = ImGui::GetContentRegionAvail();
    int w = std::max(1, (int)avail.x);
    int h = std::max(1, (int)avail.y);

    ensureFBO(w, h);

    // Render scene into FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glViewport(0, 0, texW_, texH_);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    float proj[16];
    float aspect = (float)texW_ / (float)texH_;
    ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f, proj);

    shader.use();
    shader.setMat4("uProjection", proj);
    shader.setVec2("uOffset", 0.0f, 0.0f);
    shader.setVec4("uColor", 1.0f, 0.5f, 0.2f, 1.0f);
    for (auto* m : meshes) m->draw();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Display texture in ImGui (flip vertically)
    ImGui::Image((ImTextureID)(intptr_t)colorTex_, ImVec2((float)texW_, (float)texH_), ImVec2(0,1), ImVec2(1,0));
    ImGui::EndChild();
}
