#ifndef APPLICATION_H
#define APPLICATION_H

#include <vector>
struct GLFWwindow;
class Shader;
class Mesh;
class InspectorPanel;
class ViewportPanel;
class ProjectPanel;
class PropertiesPanel;
class Scene;

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
    Scene* scene_ = nullptr;
    InspectorPanel* inspector_ = nullptr;
    ViewportPanel* viewport_ = nullptr;
    ProjectPanel* project_ = nullptr;
    PropertiesPanel* properties_ = nullptr;
};

#endif
