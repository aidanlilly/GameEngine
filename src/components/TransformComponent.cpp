#include "components/TransformComponent.h"
#include "imgui.h"

TransformComponent::TransformComponent() {
    // Default values already set in header
}

void TransformComponent::renderInspectorGUI() {
    ImGui::DragFloat3("Position", &x, 0.01f);
    ImGui::DragFloat3("Rotation", &rotX, 1.0f);
    ImGui::DragFloat3("Scale", &scaleX, 0.01f, 0.01f, 10.0f);
}
