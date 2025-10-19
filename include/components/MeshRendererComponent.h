// Moved from include/MeshRendererComponent.h
#ifndef MESH_RENDERER_COMPONENT_H
#define MESH_RENDERER_COMPONENT_H

#include "components/Component.h"

class Mesh;

class MeshRendererComponent : public Component {
public:
    MeshRendererComponent();
    ~MeshRendererComponent();
    
    std::string getTypeName() const override { return "Mesh Renderer"; }
    void renderInspectorGUI() override;
    bool canBeRemoved() const override { return true; }
    
    Mesh* mesh = nullptr;
    
    // Preset selection for quick assignment of built-in meshes
    enum class Preset { None = 0, Cube, Pyramid, Sphere };
    Preset preset = Preset::None;
    
private:
    void rebuildMesh();
    
    // Future: material properties, etc.
};

#endif