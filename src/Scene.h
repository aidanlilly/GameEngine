#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <utility>
class Mesh;

struct Transform {
    float x = 0.0f, y = 0.0f, z = 0.0f;      // position
    float rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f; // rotation in degrees (Euler angles)
    float scaleX = 1.0f, scaleY = 1.0f, scaleZ = 1.0f; // scale
};

struct MeshInstance {
    Mesh* mesh = nullptr;
    Transform transform;
};

class Scene {
public:
    Scene();
    ~Scene();
    std::vector<MeshInstance>& getInstances() { return instances_; }
    const std::vector<MeshInstance>& getInstances() const { return instances_; }
    
    // Selection
    int getSelectedIndex() const { return selectedIndex_; }
    void setSelectedIndex(int idx) { selectedIndex_ = idx; }
    MeshInstance* getSelectedInstance();
    void deleteSelected();
    
    // Helpers for clarity
    void addTriangle(float size = 1.0f, float x = 0.0f, float y = 0.0f, float z = -2.0f);
    void addSquare(float size = 1.0f, float x = 0.0f, float y = 0.0f, float z = -2.0f);
    void addCircle(float diameter = 1.0f, int segments = 32, float x = 0.0f, float y = 0.0f, float z = -2.0f);
private:
    std::vector<MeshInstance> instances_;
    int selectedIndex_ = -1;
};

#endif
