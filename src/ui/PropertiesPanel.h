#ifndef PROPERTIES_PANEL_H
#define PROPERTIES_PANEL_H

class Scene;
class Component;

class PropertiesPanel {
public:
    // Renders component properties for selected GameObject
    // Shows all components with ability to add/remove
    void render(Scene* scene);
    
private:
    // Render a single component with its header
    void renderComponent(Component* component, bool canRemove);
    
    // Show the "Add Component" button and menu
    void renderAddComponentMenu(class GameObject* gameObject);
    
    // Track which component to remove (deferred deletion)
    Component* componentToRemove_ = nullptr;
};

#endif
