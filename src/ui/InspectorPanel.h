#ifndef INSPECTOR_PANEL_H
#define INSPECTOR_PANEL_H

#include <vector>
class Mesh;

class InspectorPanel {
public:
    void render(const std::vector<Mesh*>& meshes);
};

#endif
