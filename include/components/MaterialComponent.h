#ifndef MATERIAL_COMPONENT_H
#define MATERIAL_COMPONENT_H

#include "components/Component.h"

class MaterialComponent : public Component {
public:
    MaterialComponent() {}

    std::string getTypeName() const override { return "Material"; }
    void renderInspectorGUI() override;

    // PBR-like parameters
    float albedo[3] = {0.8f, 0.5f, 0.2f};
    float metallic = 0.0f;   // 0 = dielectric, 1 = metal
    float roughness = 0.8f;  // 0 = smooth, 1 = rough
};

#endif
