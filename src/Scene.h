#ifndef SCENE_H
#define SCENE_H

#include <vector>
class Mesh;

class Scene {
public:
    Scene();
    ~Scene();
    std::vector<Mesh*>& getMeshes() { return meshes_; }
private:
    std::vector<Mesh*> meshes_;
};

#endif
