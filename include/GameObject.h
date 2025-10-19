#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>
#include <vector>
#include <memory>
#include <typeindex>
#include <unordered_map>

class Component;
class TransformComponent;

class GameObject {
public:
    GameObject(const std::string& name = "GameObject");
    ~GameObject();
    
    // Name
    std::string getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    
    // Component management
    template<typename T>
    T* addComponent();
    
    template<typename T>
    T* getComponent();
    
    template<typename T>
    bool hasComponent();
    
    void removeComponent(Component* component);
    
    std::vector<std::unique_ptr<Component>>& getComponents() { return components_; }
    const std::vector<std::unique_ptr<Component>>& getComponents() const { return components_; }
    
    // Quick access to transform (always present)
    TransformComponent* getTransform() { return transform_; }
    const TransformComponent* getTransform() const { return transform_; }
    
private:
    std::string name_;
    std::vector<std::unique_ptr<Component>> components_;
    std::unordered_map<std::type_index, Component*> componentMap_;
    TransformComponent* transform_; // Cached pointer to transform
};

// Template implementations
template<typename T>
T* GameObject::addComponent() {
    static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
    
    // Check if component already exists (one per type)
    std::type_index typeIdx(typeid(T));
    if (componentMap_.find(typeIdx) != componentMap_.end()) {
        return static_cast<T*>(componentMap_[typeIdx]);
    }
    
    auto component = std::make_unique<T>();
    T* ptr = component.get();
    ptr->gameObject = this;
    
    componentMap_[typeIdx] = ptr;
    components_.push_back(std::move(component));
    
    return ptr;
}

template<typename T>
T* GameObject::getComponent() {
    static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
    
    std::type_index typeIdx(typeid(T));
    auto it = componentMap_.find(typeIdx);
    if (it != componentMap_.end()) {
        return static_cast<T*>(it->second);
    }
    return nullptr;
}

template<typename T>
bool GameObject::hasComponent() {
    static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
    
    std::type_index typeIdx(typeid(T));
    return componentMap_.find(typeIdx) != componentMap_.end();
}

#endif
