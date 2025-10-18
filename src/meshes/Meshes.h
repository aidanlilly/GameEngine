#ifndef MESHES_H
#define MESHES_H

#include "Mesh.h"
#include <vector>

inline Mesh* CreateTriangleMesh(float size = 1.0f) {
    float s = size * 0.5f;
    std::vector<Vertex> verts = {
        { 0.0f,  s, 0.0f},
        {-s, -s, 0.0f},
        { s, -s, 0.0f}
    };
    return new Mesh(verts);
}

inline Mesh* CreateSquareMesh(float size = 1.0f) {
    float s = size * 0.5f;
    std::vector<Vertex> verts = {
        {-s,  s, 0.0f},
        {-s, -s, 0.0f},
        { s, -s, 0.0f},
        { s, -s, 0.0f},
        { s,  s, 0.0f},
        {-s,  s, 0.0f}
    };
    return new Mesh(verts);
}

#endif
