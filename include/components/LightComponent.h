// Moved from include/LightComponent.h
#ifndef LIGHT_COMPONENT_H
#define LIGHT_COMPONENT_H

#include "components/Component.h"

// Example component to demonstrate extensibility
class LightComponent : public Component {
public:
    LightComponent();
    
    std::string getTypeName() const override { return "Light"; }
    void renderInspectorGUI() override;
    bool canBeRemoved() const override { return true; }
    
    // Light properties
    float color[3] = { 1.0f, 1.0f, 1.0f }; // RGB
    float intensity = 1.0f;
    float range = 10.0f;
    
    enum class Type { Point, Directional, Spot };
    Type type = Type::Point;
};

#endif