#ifndef MESH_H
#define MESH_H

#include <vector>
#include <GL/glew.h>

struct Vertex {
    float x, y, z;
};

class Mesh {
public:
    // Constructor
    Mesh(const std::vector<Vertex>& vertices);
    
    // Destructor
    ~Mesh();

    // Render the mesh
    void draw() const;

    // Get vertex count
    unsigned int getVertexCount() const { return vertexCount; }
    
    // Get vertex data for ray intersection
    const std::vector<Vertex>& getVertices() const { return vertices_; }

private:
    unsigned int VAO, VBO;
    unsigned int vertexCount;
    std::vector<Vertex> vertices_; // Store for ray-casting

    // Setup mesh buffers
    void setupMesh(const std::vector<Vertex>& vertices);
};

#endif
