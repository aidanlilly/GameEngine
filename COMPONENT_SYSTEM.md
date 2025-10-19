# Component System Guide

## Overview
The game engine now uses a component-based architecture similar to Unity. GameObjects hold multiple components that define their behavior and properties.

## Core Classes

### `Component` (Base Class)
All components inherit from this base class. Located in `include/Component.h`.

**Key Methods:**
- `getTypeName()` - Returns the display name shown in the inspector
- `renderInspectorGUI()` - Renders the component's properties in the inspector panel
- `canBeRemoved()` - Returns true if the component can be removed (Transform cannot)

### `GameObject`
Represents an entity in the scene. Located in `include/GameObject.h`.

**Key Methods:**
- `addComponent<T>()` - Adds a component of type T to the GameObject
- `getComponent<T>()` - Gets a component of type T (returns nullptr if not found)
- `hasComponent<T>()` - Checks if GameObject has a component of type T
- `removeComponent(Component*)` - Removes a specific component instance
- `getTransform()` - Quick access to the Transform component (always present)

## Built-in Components

### `TransformComponent`
Every GameObject has this component (cannot be removed). Stores position, rotation, and scale.

**Properties:**
- `x, y, z` - Position
- `rotX, rotY, rotZ` - Rotation (Euler angles in degrees)
- `scaleX, scaleY, scaleZ` - Scale

### `MeshRendererComponent`
Renders a 3D mesh. Contains a pointer to a Mesh object.

**Properties:**
- `mesh` - Pointer to the Mesh to render

### `LightComponent`
Example component demonstrating extensibility. Defines light properties.

**Properties:**
- `color` - RGB color
- `intensity` - Light intensity (0-10)
- `range` - Light range
- `type` - Point, Directional, or Spot

## Creating New Components

### Step 1: Create Header File
Create a new header in `include/YourComponent.h`:

```cpp
#ifndef YOUR_COMPONENT_H
#define YOUR_COMPONENT_H

#include "Component.h"

class YourComponent : public Component {
public:
    YourComponent();
    
    std::string getTypeName() const override { return "Your Component"; }
    void renderInspectorGUI() override;
    bool canBeRemoved() const override { return true; }
    
    // Your component properties here
    float someProperty = 0.0f;
};

#endif
```

### Step 2: Create Implementation File
Create `src/YourComponent.cpp`:

```cpp
#include "YourComponent.h"
#include "imgui.h"

YourComponent::YourComponent() {
    // Initialize your component
}

void YourComponent::renderInspectorGUI() {
    // Use ImGui to render your properties
    ImGui::DragFloat("Some Property", &someProperty, 0.01f);
}
```

### Step 3: Add to CMakeLists.txt
Add your source file to the `SOURCES` list:

```cmake
set(SOURCES
    ...
    src/YourComponent.cpp
    ...
)
```

### Step 4: Register in PropertiesPanel
Edit `src/ui/PropertiesPanel.cpp`:

1. Add include at the top:
```cpp
#include "YourComponent.h"
```

2. Add to the `renderAddComponentMenu()` function:
```cpp
// Your Component
if (!gameObject->hasComponent<YourComponent>()) {
    if (ImGui::Selectable("Your Component")) {
        gameObject->addComponent<YourComponent>();
        ImGui::CloseCurrentPopup();
    }
} else {
    ImGui::TextDisabled("Your Component (already added)");
}
```

## Using the Component System

### In the Inspector Panel
- Select a GameObject from the hierarchy
- View all components in the Properties panel
- Each component has a collapsible header showing its properties
- Click the "X" button to remove a component (except Transform)
- Click "Add Component" to add new components from the menu

### In Code
```cpp
// Get a GameObject
GameObject* go = scene->getSelectedGameObject();

// Add a component
auto* light = go->addComponent<LightComponent>();
light->intensity = 2.0f;

// Get a component
auto* transform = go->getTransform();
transform->x = 5.0f;

// Check if has component
if (go->hasComponent<MeshRendererComponent>()) {
    auto* renderer = go->getComponent<MeshRendererComponent>();
    // Use renderer...
}

// Remove a component
auto* light = go->getComponent<LightComponent>();
if (light) {
    go->removeComponent(light);
}
```

## Architecture Benefits

1. **Modularity**: Each component handles a specific aspect of functionality
2. **Flexibility**: Mix and match components to create different object types
3. **Extensibility**: Easy to add new components without modifying existing code
4. **Type Safety**: Template-based system ensures type safety at compile time
5. **Unity-like**: Familiar workflow for Unity developers

## Future Enhancements

Consider adding:
- `RigidbodyComponent` for physics
- `ScriptComponent` for custom behavior
- `AudioSourceComponent` for sound
- `CameraComponent` for multiple cameras
- `ColliderComponent` for collision detection
- Component serialization for save/load functionality
