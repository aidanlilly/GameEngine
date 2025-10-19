#include "Grid.h"
#include "Shader.h"
#include "MathUtils.h"
#include <vector>

Grid::Grid(int size, float spacing)
    : vao_(0)
    , vbo_(0)
    , vertexCount_(0)
    , gridSize_(size)
    , gridSpacing_(spacing)
{
    // Make the grid much larger to support infinite-looking coverage
    gridSize_ = std::max(gridSize_, 500);
    createMesh();
}

Grid::~Grid() {
    destroyMesh();
}

void Grid::createMesh() {
    // Create a grid on the XZ plane (Y=0)
    std::vector<float> vertices;

    // Lines parallel to X axis
    for (int z = -gridSize_; z <= gridSize_; ++z) {
        vertices.push_back(-gridSize_ * gridSpacing_);
        vertices.push_back(0.0f);
        vertices.push_back(z * gridSpacing_);
        
        vertices.push_back(gridSize_ * gridSpacing_);
        vertices.push_back(0.0f);
        vertices.push_back(z * gridSpacing_);
    }

    // Lines parallel to Z axis
    for (int x = -gridSize_; x <= gridSize_; ++x) {
        vertices.push_back(x * gridSpacing_);
        vertices.push_back(0.0f);
        vertices.push_back(-gridSize_ * gridSpacing_);
        
        vertices.push_back(x * gridSpacing_);
        vertices.push_back(0.0f);
        vertices.push_back(gridSize_ * gridSpacing_);
    }

    vertexCount_ = vertices.size() / 3;

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Grid::destroyMesh() {
    if (vao_) glDeleteVertexArrays(1, &vao_);
    if (vbo_) glDeleteBuffers(1, &vbo_);
    vao_ = 0;
    vbo_ = 0;
    vertexCount_ = 0;
}

void Grid::render(Shader& shader) {
    // Expect caller to set uModel for placement; we only set the color and draw.
    shader.setVec4("uColor", 0.3f, 0.3f, 0.3f, 1.0f);
    glBindVertexArray(vao_);
    glDrawArrays(GL_LINES, 0, vertexCount_);
    glBindVertexArray(0);
}
