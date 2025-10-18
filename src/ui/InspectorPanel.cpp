#include "InspectorPanel.h"
#include "Mesh.h"
#include "../Scene.h"
#include "imgui.h"
#include <cstdio>

void InspectorPanel::render(Scene* scene) {
    ImGui::BeginChild("Inspector", ImVec2(0, 0), true);
    ImGui::Text("Scene Hierarchy");
    ImGui::Separator();
    
    if (!scene) {
        ImGui::TextDisabled("No scene");
        ImGui::EndChild();
        return;
    }
    
    auto& instances = scene->getInstances();
    ImGui::Text("Objects: %zu", instances.size());
    ImGui::Separator();
    
    // List instances (clickable to select)
    for (size_t i = 0; i < instances.size(); ++i) {
        char label[64];
        snprintf(label, sizeof(label), "Object %zu (%u verts)", i, instances[i].mesh->getVertexCount());
        if (ImGui::Selectable(label, scene->getSelectedIndex() == (int)i)) {
            scene->setSelectedIndex((int)i);
        }
    }
    
    ImGui::EndChild();
}
