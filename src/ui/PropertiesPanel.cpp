#include "PropertiesPanel.h"
#include "../Scene.h"
#include "imgui.h"

void PropertiesPanel::render(Scene* scene) {
    ImGui::BeginChild("Properties", ImVec2(0, 0), true);
    ImGui::Text("Properties");
    ImGui::Separator();

    if (!scene) {
        ImGui::TextDisabled("No scene loaded");
        ImGui::EndChild();
        return;
    }

    MeshInstance* selected = scene->getSelectedInstance();
    if (!selected) {
        ImGui::TextDisabled("No object selected");
        ImGui::EndChild();
        return;
    }

    ImGui::Text("Transform:");
    // Position (3D)
    ImGui::DragFloat3("Position", &selected->transform.x, 0.01f);
    // Rotation (Euler angles, degrees)
    ImGui::DragFloat3("Rotation (deg)", &selected->transform.rotX, 1.0f);
    // Scale (3D)
    ImGui::DragFloat3("Scale", &selected->transform.scaleX, 0.01f, 0.01f, 10.0f);

    ImGui::EndChild();
}
