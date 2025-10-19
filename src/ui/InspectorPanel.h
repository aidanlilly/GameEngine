#ifndef INSPECTOR_PANEL_H
#define INSPECTOR_PANEL_H

#include <vector>
class Mesh;
class Scene;

class InspectorPanel {
public:
    void render(Scene* scene);
    
private:
    int renamingIndex_ = -1;
    char renameBuffer_[128] = "";
};

#endif
