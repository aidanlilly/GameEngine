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

private:
    unsigned int VAO, VBO;
    unsigned int vertexCount;

    // Setup mesh buffers
    void setupMesh(const std::vector<Vertex>& vertices);
};

#endif
