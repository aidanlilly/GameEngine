#ifndef VIEWPORT_PANEL_H
#define VIEWPORT_PANEL_H

#include <GL/glew.h>
#include <vector>

class Mesh;
class Shader;
class Scene;

class ViewportPanel {
public:
    ViewportPanel();
    ~ViewportPanel();

    void render(Shader& shader, Scene* scene);

private:
    void ensureFBO(int w, int h);
    void destroyFBO();
    void createGridMesh();
    void destroyGridMesh();
    void renderGrid(Shader& shader);

    GLuint fbo_ = 0;
    GLuint colorTex_ = 0;
    GLuint depthRbo_ = 0;
    int texW_ = 0;
    int texH_ = 0;

    // Grid mesh
    GLuint gridVAO_ = 0;
    GLuint gridVBO_ = 0;
    int gridVertexCount_ = 0;

    // 3D Camera
    float camDistance_ = 10.0f;
    float camYaw_ = 0.0f;
    float camPitch_ = 0.3f;
    float camPanX_ = 0.0f;
    float camPanY_ = 0.0f;
};

#endif
