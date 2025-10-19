#ifndef GRID_H
#define GRID_H

#include <GL/glew.h>

class Shader;

class Grid {
public:
    Grid(int size = 20, float spacing = 1.0f);
    ~Grid();
    
    void render(Shader& shader);
    
private:
    void createMesh();
    void destroyMesh();
    
    GLuint vao_;
    GLuint vbo_;
    int vertexCount_;
    int gridSize_;
    float gridSpacing_;
};

#endif
