#include "InspectorPanel.h"
#include "Mesh.h"
#include "imgui.h"

void InspectorPanel::render(const std::vector<Mesh*>& meshes) {
    ImGui::BeginChild("Inspector", ImVec2(0, 0), true);
    ImGui::Text("Mesh Inspector");
    ImGui::Separator();
    for (size_t i = 0; i < meshes.size(); ++i) {
        ImGui::Text("Mesh %zu: %u vertices", i, meshes[i]->getVertexCount());
    }
    ImGui::EndChild();
}
