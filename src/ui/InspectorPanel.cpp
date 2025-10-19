#include "InspectorPanel.h"
#include "../Scene.h"
#include "GameObject.h"
#include "components/MeshRendererComponent.h"
#include "Mesh.h"
#include "imgui.h"
#include <cstdio>
#include <cstring>

void InspectorPanel::render(Scene* scene) {
    ImGui::BeginChild("Inspector", ImVec2(0, 0), true);
    ImGui::Text("Scene Hierarchy");
    ImGui::Separator();
    
    if (!scene) {
        ImGui::TextDisabled("No scene");
        ImGui::EndChild();
        return;
    }
    
    auto& gameObjects = scene->getGameObjects();
    ImGui::Text("Objects: %zu", gameObjects.size());
    ImGui::Separator();
    
    // List instances (clickable to select)
    for (size_t i = 0; i < gameObjects.size(); ++i) {
        ImGui::PushID((int)i);
        
        // Display object name with vertex count
        char label[128];
        auto* meshRenderer = gameObjects[i]->getComponent<MeshRendererComponent>();
        unsigned int vertCount = (meshRenderer && meshRenderer->mesh) ? meshRenderer->mesh->getVertexCount() : 0;
        snprintf(label, sizeof(label), "%s (%u verts)", 
                 gameObjects[i]->getName().c_str(), 
                 vertCount);
        
        bool isSelected = (scene->getSelectedIndex() == (int)i);
        if (ImGui::Selectable(label, isSelected)) {
            scene->setSelectedIndex((int)i);
        }
        
        // Right-click context menu
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Rename")) {
                renamingIndex_ = (int)i;
                // Copy current name to buffer
                strncpy(renameBuffer_, gameObjects[i]->getName().c_str(), sizeof(renameBuffer_) - 1);
                renameBuffer_[sizeof(renameBuffer_) - 1] = '\0';
            }
            ImGui::EndPopup();
        }
        
        ImGui::PopID();
    }
    
    // Rename popup modal
    if (renamingIndex_ >= 0 && renamingIndex_ < (int)gameObjects.size()) {
        ImGui::OpenPopup("Rename Object");
        if (ImGui::BeginPopupModal("Rename Object", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Enter new name:");
            ImGui::SetKeyboardFocusHere();
            bool enterPressed = ImGui::InputText("##rename", renameBuffer_, sizeof(renameBuffer_), 
                                                  ImGuiInputTextFlags_EnterReturnsTrue);
            
            if (enterPressed || ImGui::Button("OK")) {
                gameObjects[renamingIndex_]->setName(renameBuffer_);
                renamingIndex_ = -1;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel")) {
                renamingIndex_ = -1;
                ImGui::CloseCurrentPopup();
            }
            
            ImGui::EndPopup();
        }
    }
    
    // Right-click on empty space of the Inspector to create a new GameObject
    // Combine MouseButtonRight to avoid opening on left-click when flags are provided
    if (ImGui::BeginPopupContextWindow("InspectorContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
        if (ImGui::MenuItem("Create Empty")) {
            scene->addEmptyGameObject("GameObject", 0.0f, 0.0f, 0.0f);
        }
        ImGui::EndPopup();
    }
    
    ImGui::EndChild();
}
