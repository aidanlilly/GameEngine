#include "Scene.h"
#include "Mesh.h"
#include "Meshes.h"
#include <algorithm>

Scene::Scene() {
    // Start with empty scene
}

Scene::~Scene() {
    for (auto& inst : instances_) delete inst.mesh;
    instances_.clear();
}

MeshInstance* Scene::getSelectedInstance() {
    if (selectedIndex_ >= 0 && selectedIndex_ < (int)instances_.size()) {
        return &instances_[selectedIndex_];
    }
    return nullptr;
}

void Scene::deleteSelected() {
    if (selectedIndex_ >= 0 && selectedIndex_ < (int)instances_.size()) {
        delete instances_[selectedIndex_].mesh;
        instances_.erase(instances_.begin() + selectedIndex_);
        selectedIndex_ = -1;
    }
}

std::string Scene::generateUniqueName(const std::string& baseName) {
    // Check if base name exists
    bool exists = false;
    for (const auto& inst : instances_) {
        if (inst.name == baseName) {
            exists = true;
            break;
        }
    }
    
    if (!exists) return baseName;
    
    // Find highest number suffix
    int maxNum = 0;
    for (const auto& inst : instances_) {
        // Check if name starts with baseName
        if (inst.name.find(baseName) == 0) {
            // Look for " (#)" pattern
            size_t parenPos = inst.name.find(" (");
            if (parenPos != std::string::npos) {
                size_t closePos = inst.name.find(")", parenPos);
                if (closePos != std::string::npos) {
                    std::string numStr = inst.name.substr(parenPos + 2, closePos - parenPos - 2);
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
    MeshInstance inst;
    inst.mesh = CreatePyramidMesh(size);
    inst.name = generateUniqueName(baseName);
    inst.transform.x = x;
    inst.transform.y = y;
    inst.transform.z = z;
    inst.transform.scaleX = inst.transform.scaleY = inst.transform.scaleZ = 1.0f;
    instances_.push_back(inst);
}

void Scene::addCube(float size, float x, float y, float z, const std::string& baseName) {
    MeshInstance inst;
    inst.mesh = CreateCubeMesh(size);
    inst.name = generateUniqueName(baseName);
    inst.transform.x = x;
    inst.transform.y = y;
    inst.transform.z = z;
    inst.transform.scaleX = inst.transform.scaleY = inst.transform.scaleZ = 1.0f;
    instances_.push_back(inst);
}

void Scene::addSphere(float diameter, int segments, float x, float y, float z, const std::string& baseName) {
    MeshInstance inst;
    inst.mesh = CreateSphereMesh(diameter, segments);
    inst.name = generateUniqueName(baseName);
    inst.transform.x = x;
    inst.transform.y = y;
    inst.transform.z = z;
    inst.transform.scaleX = inst.transform.scaleY = inst.transform.scaleZ = 1.0f;
    instances_.push_back(inst);
}
