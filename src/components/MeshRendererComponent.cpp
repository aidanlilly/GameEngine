#include "components/MeshRendererComponent.h"
#include "Mesh.h"
#include "Meshes.h"
#include "imgui.h"

MeshRendererComponent::MeshRendererComponent() {
}

MeshRendererComponent::~MeshRendererComponent() {
    // Mesh is owned by this component
    if (mesh) {
        delete mesh;
        mesh = nullptr;
    }
}

void MeshRendererComponent::renderInspectorGUI() {
    // Preset selector
    const char* presetNames[] = { "None", "Cube", "Pyramid", "Sphere" };
    int presetIndex = static_cast<int>(preset);
    if (ImGui::Combo("Preset", &presetIndex, presetNames, IM_ARRAYSIZE(presetNames))) {
        preset = static_cast<Preset>(presetIndex);
        rebuildMesh();
    }

    // No size/segments controls; presets use defaults

    ImGui::Separator();
    if (mesh) {
        ImGui::Text("Mesh: Active");
        ImGui::Text("Vertices: %d", mesh->getVertexCount());
        ImGui::Text("Triangles: %d", mesh->getVertexCount() / 3);
        if (ImGui::Button("Clear Mesh")) {
            delete mesh; mesh = nullptr;
            preset = Preset::None;
        }
    } else {
        ImGui::TextDisabled("No mesh assigned");
    }

    // Future: Add material selection, color picker, etc.
}

void MeshRendererComponent::rebuildMesh() {
    // Clean up existing mesh if any
    if (mesh) { delete mesh; mesh = nullptr; }

    switch (preset) {
        case Preset::None:
            mesh = nullptr;
            break;
        case Preset::Cube:
            mesh = CreateCubeMesh(1.0f);
            break;
        case Preset::Pyramid:
            mesh = CreatePyramidMesh(1.0f);
            break;
        case Preset::Sphere:
            mesh = CreateSphereMesh(1.0f, 32);
            break;
    }
}
