#ifndef PROPERTIES_PANEL_H
#define PROPERTIES_PANEL_H

class Scene;

class PropertiesPanel {
public:
    // Renders transform properties for selected instance
    // Only shows content if something is selected
    void render(Scene* scene);
};

#endif
