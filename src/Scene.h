#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <utility>
#include <string>
class Mesh;

struct Transform {
    float x = 0.0f, y = 0.0f, z = 0.0f;      // position
    float rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f; // rotation in degrees (Euler angles)
    float scaleX = 1.0f, scaleY = 1.0f, scaleZ = 1.0f; // scale
};

struct MeshInstance {
    Mesh* mesh = nullptr;
    Transform transform;
    std::string name = "Object";
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
    void addPyramid(float size = 1.0f, float x = 0.0f, float y = 0.0f, float z = -2.0f, const std::string& baseName = "Pyramid");
    void addCube(float size = 1.0f, float x = 0.0f, float y = 0.0f, float z = -2.0f, const std::string& baseName = "Cube");
    void addSphere(float diameter = 1.0f, int segments = 32, float x = 0.0f, float y = 0.0f, float z = -2.0f, const std::string& baseName = "Sphere");
    
private:
    std::vector<MeshInstance> instances_;
    int selectedIndex_ = -1;
    
    // Generate unique name with numbering if duplicates exist
    std::string generateUniqueName(const std::string& baseName);
};

#endif
