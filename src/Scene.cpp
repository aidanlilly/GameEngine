#include "Scene.h"
#include "Mesh.h"
#include "meshes/Meshes.h"

Scene::Scene() {
    addTriangle();
}

Scene::~Scene() {
    for (auto* m : meshes_) delete m;
    meshes_.clear();
}

void Scene::addTriangle(float size) {
    meshes_.push_back(CreateTriangleMesh(size));
}

void Scene::addSquare(float size) {
    meshes_.push_back(CreateSquareMesh(size));
}
