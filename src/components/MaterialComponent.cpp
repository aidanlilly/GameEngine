#include "components/MaterialComponent.h"
#include "imgui.h"

void MaterialComponent::renderInspectorGUI() {
    ImGui::ColorEdit3("Albedo", albedo);
    ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f);
    ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f);
}
