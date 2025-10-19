#include "PropertiesPanel.h"
#include "../Scene.h"
#include "GameObject.h"
#include "components/Component.h"
#include "components/TransformComponent.h"
#include "components/MeshRendererComponent.h"
#include "components/LightComponent.h"
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

    GameObject* selected = scene->getSelectedGameObject();
    if (!selected) {
        ImGui::TextDisabled("No object selected");
        ImGui::EndChild();
        return;
    }

    // Show GameObject name
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", selected->getName().c_str());
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Spacing();

    // Handle deferred component removal
    if (componentToRemove_) {
        selected->removeComponent(componentToRemove_);
        componentToRemove_ = nullptr;
    }

    // Render all components
    auto& components = selected->getComponents();
    for (auto& component : components) {
        renderComponent(component.get(), component->canBeRemoved());
    }

    // Add Component button
    ImGui::Spacing();
    ImGui::Separator();
    renderAddComponentMenu(selected);

    ImGui::EndChild();
}

void PropertiesPanel::renderComponent(Component* component, bool canRemove) {
    if (!component) return;

    ImGui::PushID(component);
    
    // Component header with collapsible section
    bool isOpen = ImGui::CollapsingHeader(component->getTypeName().c_str(), 
        ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap);
    
    // Remove button (if allowed) on the same line as header
    if (canRemove) {
        ImGui::SameLine(ImGui::GetWindowWidth() - 35);
        if (ImGui::SmallButton("X")) {
            componentToRemove_ = component;
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Remove Component");
        }
    }
    
    // Render component's inspector GUI if expanded
    if (isOpen) {
        ImGui::Indent(10.0f);
        component->renderInspectorGUI();
        ImGui::Unindent(10.0f);
        ImGui::Spacing();
    }
    
    ImGui::PopID();
}

void PropertiesPanel::renderAddComponentMenu(GameObject* gameObject) {
    if (!gameObject) return;

    ImGui::Spacing();
    if (ImGui::Button("Add Component", ImVec2(-1, 0))) {
        ImGui::OpenPopup("AddComponentPopup");
    }

    if (ImGui::BeginPopup("AddComponentPopup")) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Select Component Type:");
        ImGui::Separator();

        // Mesh Renderer
        if (!gameObject->hasComponent<MeshRendererComponent>()) {
            if (ImGui::Selectable("Mesh Renderer")) {
                gameObject->addComponent<MeshRendererComponent>();
                ImGui::CloseCurrentPopup();
            }
        } else {
            ImGui::TextDisabled("Mesh Renderer (already added)");
        }

            // Light
            if (!gameObject->hasComponent<LightComponent>()) {
                if (ImGui::Selectable("Light")) {
                    gameObject->addComponent<LightComponent>();
                    ImGui::CloseCurrentPopup();
                }
            } else {
                ImGui::TextDisabled("Light (already added)");
            }

        // Add more components here as you create them
        // Example:
        // if (!gameObject->hasComponent<RigidbodyComponent>()) {
        //     if (ImGui::Selectable("Rigidbody")) {
        //         gameObject->addComponent<RigidbodyComponent>();
        //         ImGui::CloseCurrentPopup();
        //     }
        // }

        ImGui::EndPopup();
    }
}
