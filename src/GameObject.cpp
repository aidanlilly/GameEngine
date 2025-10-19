#include "GameObject.h"
#include "components/TransformComponent.h"
#include <algorithm>

GameObject::GameObject(const std::string& name)
    : name_(name), transform_(nullptr) {
    // Every GameObject must have a Transform component
    transform_ = addComponent<TransformComponent>();
}

GameObject::~GameObject() {
    components_.clear();
    componentMap_.clear();
}

void GameObject::removeComponent(Component* component) {
    if (!component || !component->canBeRemoved()) {
        return; // Can't remove Transform or null components
    }
    
    // Find and remove from componentMap
    for (auto it = componentMap_.begin(); it != componentMap_.end(); ++it) {
        if (it->second == component) {
            componentMap_.erase(it);
            break;
        }
    }
    
    // Find and remove from components vector
    components_.erase(
        std::remove_if(components_.begin(), components_.end(),
            [component](const std::unique_ptr<Component>& c) {
                return c.get() == component;
            }),
        components_.end()
    );
}
