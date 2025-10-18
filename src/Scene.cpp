#include "Scene.h"
#include "Mesh.h"

Scene::Scene() {
    float aspect = 800.0f / 600.0f;
    std::vector<Vertex> triangleVertices = {
        { 0.0f,  0.5f, 0.0f},
        {-0.5f, -0.5f, 0.0f},
        { 0.5f, -0.5f, 0.0f}
    };
    meshes_.push_back(new Mesh(triangleVertices));

    std::vector<Vertex> squareVertices = {
        {-0.5f * aspect,  0.5f, 0.0f},
        {-0.5f * aspect, -0.5f, 0.0f},
        { 0.5f * aspect, -0.5f, 0.0f},
        { 0.5f * aspect, -0.5f, 0.0f},
        { 0.5f * aspect,  0.5f, 0.0f},
        {-0.5f * aspect,  0.5f, 0.0f}
    };
    meshes_.push_back(new Mesh(squareVertices));
}

Scene::~Scene() {
    for (auto* m : meshes_) delete m;
    meshes_.clear();
}
