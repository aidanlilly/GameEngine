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

    // Renders the given scene into an offscreen texture sized to available content region, then displays it.
    // Also accepts drag/drop of mesh assets and click-to-select.
    void render(Shader& shader, Scene* scene);

private:
    void ensureFBO(int w, int h);
    void destroyFBO();

    GLuint fbo_ = 0;
    GLuint colorTex_ = 0;
    GLuint depthRbo_ = 0;
    int texW_ = 0;
    int texH_ = 0;
};

#endif
