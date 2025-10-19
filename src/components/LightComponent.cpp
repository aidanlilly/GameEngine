#include "components/LightComponent.h"
#include "imgui.h"

LightComponent::LightComponent() {
    // Default values already set in header
}

void LightComponent::renderInspectorGUI() {
    // Type selector
    const char* typeNames[] = { "Point", "Directional", "Spot", "Ambient" };
    int currentType = (int)type;
    if (ImGui::Combo("Type", &currentType, typeNames, 4)) {
        type = (Type)currentType;
    }
    
    // Color picker
    ImGui::ColorEdit3("Color", color);
    
    // Intensity slider
    ImGui::SliderFloat("Intensity", &intensity, 0.0f, 10.0f);
    
    // Range (only for Point and Spot lights)
    if (type == Type::Point || type == Type::Spot) {
        ImGui::DragFloat("Range", &range, 0.1f, 0.1f, 100.0f);
    }
}
