#include "Scene.h"
#include "Mesh.h"
#include "Meshes.h"

Scene::Scene() {
    addTriangle(2.0f, -1.0f, 0.0f, 0.0f);
    addSquare(2.0f, 1.0f, 0.0f, 0.0f);
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

void Scene::addTriangle(float size, float x, float y, float z) {
    MeshInstance inst;
    inst.mesh = CreateTriangleMesh(size);
    inst.transform.x = x;
    inst.transform.y = y;
    inst.transform.z = z;
    inst.transform.scaleX = inst.transform.scaleY = inst.transform.scaleZ = 1.0f;
    instances_.push_back(inst);
}

void Scene::addSquare(float size, float x, float y, float z) {
    MeshInstance inst;
    inst.mesh = CreateSquareMesh(size);
    inst.transform.x = x;
    inst.transform.y = y;
    inst.transform.z = z;
    inst.transform.scaleX = inst.transform.scaleY = inst.transform.scaleZ = 1.0f;
    instances_.push_back(inst);
}

void Scene::addCircle(float diameter, int segments, float x, float y, float z) {
    MeshInstance inst;
    inst.mesh = CreateCircleMesh(diameter, segments);
    inst.transform.x = x;
    inst.transform.y = y;
    inst.transform.z = z;
    inst.transform.scaleX = inst.transform.scaleY = inst.transform.scaleZ = 1.0f;
    instances_.push_back(inst);
}
