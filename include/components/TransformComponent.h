// Moved from include/TransformComponent.h
#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include "components/Component.h"

class TransformComponent : public Component {
public:
    TransformComponent();
    
    std::string getTypeName() const override { return "Transform"; }
    void renderInspectorGUI() override;
    bool canBeRemoved() const override { return false; } // Transform cannot be removed
    
    // Position
    float x = 0.0f, y = 0.0f, z = 0.0f;
    
    // Rotation (Euler angles in degrees)
    float rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f;
    
    // Scale
    float scaleX = 1.0f, scaleY = 1.0f, scaleZ = 1.0f;
};

#endif