#ifndef VIEWPORT_PANEL_H
#define VIEWPORT_PANEL_H

#include <GL/glew.h>
#include <memory>

class Shader;
class Scene;
class Camera;
class Grid;

class ViewportPanel {
public:
    ViewportPanel();
    ~ViewportPanel();

    void render(Shader& shader, Scene* scene);
    
    Camera& getCamera() { return *camera_; }

private:
    void ensureFBO(int w, int h);
    void destroyFBO();
    void handleCameraControls();
    void renderScene(Shader& shader, Scene* scene);
    void handleSelection(Scene* scene, bool isHovered);
    void handleDragDrop(Scene* scene);

    // Framebuffer for offscreen rendering
    GLuint fbo_ = 0;
    GLuint colorTex_ = 0;
    GLuint depthRbo_ = 0;
    int texW_ = 0;
    int texH_ = 0;

    // Scene components
    std::unique_ptr<Camera> camera_;
    std::unique_ptr<Grid> grid_;
};

#endif
