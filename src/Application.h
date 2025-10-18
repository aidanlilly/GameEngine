#ifndef APPLICATION_H
#define APPLICATION_H

#include <vector>
struct GLFWwindow;
class Shader;
class Mesh;
class InspectorPanel;
class ViewportPanel;

class Application {
public:
    Application();
    ~Application();
    int run();

private:
    bool init();
    void shutdown();
    void frame();

    GLFWwindow* window_ = nullptr;
    Shader* shader_ = nullptr;
    std::vector<Mesh*> meshes_;
    InspectorPanel* inspector_ = nullptr;
    ViewportPanel* viewport_ = nullptr;
};

#endif
