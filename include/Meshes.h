#ifndef MESHES_H
#define MESHES_H

#include "Mesh.h"
#include <vector>
#include <cmath>


// 3D pyramid (triangle base)
inline Mesh* CreateTriangleMesh(float size = 1.0f) {
    float s = size * 0.5f;
    std::vector<Vertex> verts = {
        // Base triangle (XY plane)
        { 0.0f,  s, 0.0f},
        {-s, -s, 0.0f},
        { s, -s, 0.0f},
        // Side 1
        { 0.0f,  s, 0.0f},
        {-s, -s, 0.0f},
        { 0.0f, 0.0f, size },
        // Side 2
        {-s, -s, 0.0f},
        { s, -s, 0.0f},
        { 0.0f, 0.0f, size },
        // Side 3
        { s, -s, 0.0f},
        { 0.0f,  s, 0.0f},
        { 0.0f, 0.0f, size }
    };
    return new Mesh(verts);
}


// 3D cube
inline Mesh* CreateSquareMesh(float size = 1.0f) {
    float s = size * 0.5f;
    std::vector<Vertex> verts = {
        // Front face
        {-s, -s,  s}, { s, -s,  s}, { s,  s,  s},
        { s,  s,  s}, {-s,  s,  s}, {-s, -s,  s},
        // Back face
        {-s, -s, -s}, {-s,  s, -s}, { s,  s, -s},
        { s,  s, -s}, { s, -s, -s}, {-s, -s, -s},
        // Left face
        {-s, -s, -s}, {-s, -s,  s}, {-s,  s,  s},
        {-s,  s,  s}, {-s,  s, -s}, {-s, -s, -s},
        // Right face
        { s, -s, -s}, { s,  s, -s}, { s,  s,  s},
        { s,  s,  s}, { s, -s,  s}, { s, -s, -s},
        // Top face
        {-s,  s, -s}, {-s,  s,  s}, { s,  s,  s},
        { s,  s,  s}, { s,  s, -s}, {-s,  s, -s},
        // Bottom face
        {-s, -s, -s}, { s, -s, -s}, { s, -s,  s},
        { s, -s,  s}, {-s, -s,  s}, {-s, -s, -s}
    };
    return new Mesh(verts);
}


// 3D UV sphere (approximate)
inline Mesh* CreateCircleMesh(float diameter = 1.0f, int segments = 16) {
    float r = diameter * 0.5f;
    int rings = segments / 2;
    std::vector<Vertex> verts;
    for (int y = 0; y < rings; ++y) {
        float v0 = (float)y / rings;
        float v1 = (float)(y + 1) / rings;
        float theta0 = v0 * 3.1415926f;
        float theta1 = v1 * 3.1415926f;
        for (int x = 0; x < segments; ++x) {
            float u0 = (float)x / segments;
            float u1 = (float)(x + 1) / segments;
            float phi0 = u0 * 2.0f * 3.1415926f;
            float phi1 = u1 * 2.0f * 3.1415926f;
            // 4 points of quad
            Vertex p00 = { r * sinf(theta0) * cosf(phi0), r * cosf(theta0), r * sinf(theta0) * sinf(phi0) };
            Vertex p01 = { r * sinf(theta0) * cosf(phi1), r * cosf(theta0), r * sinf(theta0) * sinf(phi1) };
            Vertex p10 = { r * sinf(theta1) * cosf(phi0), r * cosf(theta1), r * sinf(theta1) * sinf(phi0) };
            Vertex p11 = { r * sinf(theta1) * cosf(phi1), r * cosf(theta1), r * sinf(theta1) * sinf(phi1) };
            // 2 triangles per quad
            verts.push_back(p00); verts.push_back(p10); verts.push_back(p11);
            verts.push_back(p00); verts.push_back(p11); verts.push_back(p01);
        }
    }
    return new Mesh(verts);
}

#endif
