// Moved from include/Component.h
#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <memory>

// Forward declarations
class GameObject;

// Base component class
class Component {
public:
    virtual ~Component() = default;
    
    // Get component type name for UI display
    virtual std::string getTypeName() const = 0;
    
    // Render ImGui interface for this component
    virtual void renderInspectorGUI() = 0;
    
    // Can this component be removed?
    virtual bool canBeRemoved() const { return true; }
    
    // Reference to owning GameObject
    GameObject* gameObject = nullptr;
    
    // Enable/disable component
    bool enabled = true;
};

#endif