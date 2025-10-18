#ifndef SCENE_H
#define SCENE_H

#include <vector>
class Mesh;

class Scene {
public:
    Scene();
    ~Scene();
    std::vector<Mesh*>& getMeshes() { return meshes_; }
    // Helpers for clarity
    void addTriangle(float size = 1.0f);
    void addSquare(float size = 1.0f);
private:
    std::vector<Mesh*> meshes_;
};

#endif
