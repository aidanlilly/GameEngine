#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>
#include <string>

class Mesh;
class GameObject;

class Scene {
public:
    Scene();
    ~Scene();

    
    std::vector<std::unique_ptr<GameObject>>& getGameObjects() { return gameObjects_; }
    const std::vector<std::unique_ptr<GameObject>>& getGameObjects() const { return gameObjects_; }
    
    // Selection
    int getSelectedIndex() const { return selectedIndex_; }
    void setSelectedIndex(int idx) { selectedIndex_ = idx; }
    GameObject* getSelectedGameObject();
    void deleteSelected();
    
    // Helpers for clarity
    void addPyramid(float size = 1.0f, float x = 0.0f, float y = 0.0f, float z = -2.0f, const std::string& baseName = "Pyramid");
    void addCube(float size = 1.0f, float x = 0.0f, float y = 0.0f, float z = -2.0f, const std::string& baseName = "Cube");
    void addSphere(float diameter = 1.0f, int segments = 32, float x = 0.0f, float y = 0.0f, float z = -2.0f, const std::string& baseName = "Sphere");
    
    // Create an empty GameObject at the given position and select it
    GameObject* addEmptyGameObject(const std::string& baseName = "GameObject", float x = 0.0f, float y = 0.0f, float z = 0.0f);
    
private:
    std::vector<std::unique_ptr<GameObject>> gameObjects_;
    int selectedIndex_ = -1;
    
    // Generate unique name with numbering if duplicates exist
    std::string generateUniqueName(const std::string& baseName);
};

#endif
