#include "Scene.h"
#include "GameObject.h"
#include "components/TransformComponent.h"
#include "components/MeshRendererComponent.h"
#include "Mesh.h"
#include "Meshes.h"
#include <algorithm>

Scene::Scene() {
    // Start with empty scene
}

Scene::~Scene() {
    gameObjects_.clear();
}

GameObject* Scene::getSelectedGameObject() {
    if (selectedIndex_ >= 0 && selectedIndex_ < (int)gameObjects_.size()) {
        return gameObjects_[selectedIndex_].get();
    }
    return nullptr;
}

void Scene::deleteSelected() {
    if (selectedIndex_ >= 0 && selectedIndex_ < (int)gameObjects_.size()) {
        gameObjects_.erase(gameObjects_.begin() + selectedIndex_);
        selectedIndex_ = -1;
    }
}

std::string Scene::generateUniqueName(const std::string& baseName) {
    // Check if base name exists
    bool exists = false;
    for (const auto& go : gameObjects_) {
        if (go->getName() == baseName) {
            exists = true;
            break;
        }
    }
    
    if (!exists) return baseName;
    
    // Find highest number suffix
    int maxNum = 0;
    for (const auto& go : gameObjects_) {
        // Check if name starts with baseName
        if (go->getName().find(baseName) == 0) {
            // Look for " (#)" pattern
            size_t parenPos = go->getName().find(" (");
            if (parenPos != std::string::npos) {
                size_t closePos = go->getName().find(")", parenPos);
                if (closePos != std::string::npos) {
                    std::string numStr = go->getName().substr(parenPos + 2, closePos - parenPos - 2);
                    try {
                        int num = std::stoi(numStr);
                        maxNum = std::max(maxNum, num);
                    } catch (...) {}
                }
            }
        }
    }
    
    return baseName + " (" + std::to_string(maxNum + 1) + ")";
}

void Scene::addPyramid(float size, float x, float y, float z, const std::string& baseName) {
    auto go = std::make_unique<GameObject>(generateUniqueName(baseName));
    
    // Set transform
    auto* transform = go->getTransform();
    transform->x = x;
    transform->y = y;
    transform->z = z;
    
    // Add mesh renderer
    auto* renderer = go->addComponent<MeshRendererComponent>();
    renderer->mesh = CreatePyramidMesh(size);
    
    gameObjects_.push_back(std::move(go));
}

void Scene::addCube(float size, float x, float y, float z, const std::string& baseName) {
    auto go = std::make_unique<GameObject>(generateUniqueName(baseName));
    
    // Set transform
    auto* transform = go->getTransform();
    transform->x = x;
    transform->y = y;
    transform->z = z;
    
    // Add mesh renderer
    auto* renderer = go->addComponent<MeshRendererComponent>();
    renderer->mesh = CreateCubeMesh(size);
    
    gameObjects_.push_back(std::move(go));
}

void Scene::addSphere(float diameter, int segments, float x, float y, float z, const std::string& baseName) {
    auto go = std::make_unique<GameObject>(generateUniqueName(baseName));
    
    // Set transform
    auto* transform = go->getTransform();
    transform->x = x;
    transform->y = y;
    transform->z = z;
    
    // Add mesh renderer
    auto* renderer = go->addComponent<MeshRendererComponent>();
    renderer->mesh = CreateSphereMesh(diameter, segments);
    
    gameObjects_.push_back(std::move(go));
}

GameObject* Scene::addEmptyGameObject(const std::string& baseName, float x, float y, float z) {
    auto go = std::make_unique<GameObject>(generateUniqueName(baseName));
    auto* transform = go->getTransform();
    transform->x = x;
    transform->y = y;
    transform->z = z;

    gameObjects_.push_back(std::move(go));
    selectedIndex_ = static_cast<int>(gameObjects_.size()) - 1;
    return gameObjects_[selectedIndex_].get();
}
