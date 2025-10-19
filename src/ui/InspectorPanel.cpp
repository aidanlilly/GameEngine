#include "InspectorPanel.h"
#include "Mesh.h"
#include "../Scene.h"
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
    
    auto& instances = scene->getInstances();
    ImGui::Text("Objects: %zu", instances.size());
    ImGui::Separator();
    
    // List instances (clickable to select)
    for (size_t i = 0; i < instances.size(); ++i) {
        ImGui::PushID((int)i);
        
        // Display object name with vertex count
        char label[128];
        snprintf(label, sizeof(label), "%s (%u verts)", 
                 instances[i].name.c_str(), 
                 instances[i].mesh->getVertexCount());
        
        bool isSelected = (scene->getSelectedIndex() == (int)i);
        if (ImGui::Selectable(label, isSelected)) {
            scene->setSelectedIndex((int)i);
        }
        
        // Right-click context menu
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Rename")) {
                renamingIndex_ = (int)i;
                // Copy current name to buffer
                strncpy(renameBuffer_, instances[i].name.c_str(), sizeof(renameBuffer_) - 1);
                renameBuffer_[sizeof(renameBuffer_) - 1] = '\0';
            }
            ImGui::EndPopup();
        }
        
        ImGui::PopID();
    }
    
    // Rename popup modal
    if (renamingIndex_ >= 0 && renamingIndex_ < (int)instances.size()) {
        ImGui::OpenPopup("Rename Object");
        if (ImGui::BeginPopupModal("Rename Object", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Enter new name:");
            ImGui::SetKeyboardFocusHere();
            bool enterPressed = ImGui::InputText("##rename", renameBuffer_, sizeof(renameBuffer_), 
                                                  ImGuiInputTextFlags_EnterReturnsTrue);
            
            if (enterPressed || ImGui::Button("OK")) {
                instances[renamingIndex_].name = renameBuffer_;
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
    
    ImGui::EndChild();
}
