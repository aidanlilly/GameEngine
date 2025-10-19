#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <cmath>

namespace MathUtils {
    // Constants
    constexpr float PI = 3.14159265358979323846f;
    constexpr float DEG_TO_RAD = PI / 180.0f;
    constexpr float RAD_TO_DEG = 180.0f / PI;
    
    // Build a 4x4 model matrix from position, rotation (Euler angles in radians), and scale
    inline void buildModelMatrix(
        float tx, float ty, float tz,
        float rx, float ry, float rz,
        float sx, float sy, float sz,
        float* out16)
    {
        // Compute rotation matrix components (ZYX order)
        float cosx = cosf(rx), sinx = sinf(rx);
        float cosy = cosf(ry), siny = sinf(ry);
        float cosz = cosf(rz), sinz = sinf(rz);
        
        // Column-major 4x4 matrix with scale, rotation, and translation
        out16[0] = cosy * cosz * sx;
        out16[1] = (sinx * siny * cosz - cosx * sinz) * sx;
        out16[2] = (cosx * siny * cosz + sinx * sinz) * sx;
        out16[3] = 0.0f;
        
        out16[4] = cosy * sinz * sy;
        out16[5] = (sinx * siny * sinz + cosx * cosz) * sy;
        out16[6] = (cosx * siny * sinz - sinx * cosz) * sy;
        out16[7] = 0.0f;
        
        out16[8] = -siny * sz;
        out16[9] = sinx * cosy * sz;
        out16[10] = cosx * cosy * sz;
        out16[11] = 0.0f;
        
        out16[12] = tx;
        out16[13] = ty;
        out16[14] = tz;
        out16[15] = 1.0f;
    }
    
    // Build identity matrix
    inline void buildIdentityMatrix(float* out16) {
        for (int i = 0; i < 16; ++i) out16[i] = 0.0f;
        out16[0] = out16[5] = out16[10] = out16[15] = 1.0f;
    }
    
    // Invert a 4x4 matrix (simplified for camera matrices)
    inline bool invertMatrix(const float* m, float* out) {
        float inv[16];
        
        inv[0] = m[5]*m[10]*m[15] - m[5]*m[11]*m[14] - m[9]*m[6]*m[15] + 
                 m[9]*m[7]*m[14] + m[13]*m[6]*m[11] - m[13]*m[7]*m[10];
        inv[4] = -m[4]*m[10]*m[15] + m[4]*m[11]*m[14] + m[8]*m[6]*m[15] - 
                 m[8]*m[7]*m[14] - m[12]*m[6]*m[11] + m[12]*m[7]*m[10];
        inv[8] = m[4]*m[9]*m[15] - m[4]*m[11]*m[13] - m[8]*m[5]*m[15] + 
                 m[8]*m[7]*m[13] + m[12]*m[5]*m[11] - m[12]*m[7]*m[9];
        inv[12] = -m[4]*m[9]*m[14] + m[4]*m[10]*m[13] + m[8]*m[5]*m[14] - 
                  m[8]*m[6]*m[13] - m[12]*m[5]*m[10] + m[12]*m[6]*m[9];
        inv[1] = -m[1]*m[10]*m[15] + m[1]*m[11]*m[14] + m[9]*m[2]*m[15] - 
                 m[9]*m[3]*m[14] - m[13]*m[2]*m[11] + m[13]*m[3]*m[10];
        inv[5] = m[0]*m[10]*m[15] - m[0]*m[11]*m[14] - m[8]*m[2]*m[15] + 
                 m[8]*m[3]*m[14] + m[12]*m[2]*m[11] - m[12]*m[3]*m[10];
        inv[9] = -m[0]*m[9]*m[15] + m[0]*m[11]*m[13] + m[8]*m[1]*m[15] - 
                 m[8]*m[3]*m[13] - m[12]*m[1]*m[11] + m[12]*m[3]*m[9];
        inv[13] = m[0]*m[9]*m[14] - m[0]*m[10]*m[13] - m[8]*m[1]*m[14] + 
                  m[8]*m[2]*m[13] + m[12]*m[1]*m[10] - m[12]*m[2]*m[9];
        inv[2] = m[1]*m[6]*m[15] - m[1]*m[7]*m[14] - m[5]*m[2]*m[15] + 
                 m[5]*m[3]*m[14] + m[13]*m[2]*m[7] - m[13]*m[3]*m[6];
        inv[6] = -m[0]*m[6]*m[15] + m[0]*m[7]*m[14] + m[4]*m[2]*m[15] - 
                 m[4]*m[3]*m[14] - m[12]*m[2]*m[7] + m[12]*m[3]*m[6];
        inv[10] = m[0]*m[5]*m[15] - m[0]*m[7]*m[13] - m[4]*m[1]*m[15] + 
                  m[4]*m[3]*m[13] + m[12]*m[1]*m[7] - m[12]*m[3]*m[5];
        inv[14] = -m[0]*m[5]*m[14] + m[0]*m[6]*m[13] + m[4]*m[1]*m[14] - 
                  m[4]*m[2]*m[13] - m[12]*m[1]*m[6] + m[12]*m[2]*m[5];
        inv[3] = -m[1]*m[6]*m[11] + m[1]*m[7]*m[10] + m[5]*m[2]*m[11] - 
                 m[5]*m[3]*m[10] - m[9]*m[2]*m[7] + m[9]*m[3]*m[6];
        inv[7] = m[0]*m[6]*m[11] - m[0]*m[7]*m[10] - m[4]*m[2]*m[11] + 
                 m[4]*m[3]*m[10] + m[8]*m[2]*m[7] - m[8]*m[3]*m[6];
        inv[11] = -m[0]*m[5]*m[11] + m[0]*m[7]*m[9] + m[4]*m[1]*m[11] - 
                  m[4]*m[3]*m[9] - m[8]*m[1]*m[7] + m[8]*m[3]*m[5];
        inv[15] = m[0]*m[5]*m[10] - m[0]*m[6]*m[9] - m[4]*m[1]*m[10] + 
                  m[4]*m[2]*m[9] + m[8]*m[1]*m[6] - m[8]*m[2]*m[5];
        
        float det = m[0]*inv[0] + m[1]*inv[4] + m[2]*inv[8] + m[3]*inv[12];
        if (fabs(det) < 1e-6f) return false;
        
        det = 1.0f / det;
        for (int i = 0; i < 16; i++) out[i] = inv[i] * det;
        return true;
    }
    
    // Ray-triangle intersection (Möller–Trumbore algorithm)
    // Returns true if hit, and outputs distance t
    inline bool rayTriangleIntersect(
        float rayOriginX, float rayOriginY, float rayOriginZ,
        float rayDirX, float rayDirY, float rayDirZ,
        float v0x, float v0y, float v0z,
        float v1x, float v1y, float v1z,
        float v2x, float v2y, float v2z,
        float& t)
    {
        const float EPSILON = 0.0000001f;
        
        // Edge vectors
        float edge1x = v1x - v0x, edge1y = v1y - v0y, edge1z = v1z - v0z;
        float edge2x = v2x - v0x, edge2y = v2y - v0y, edge2z = v2z - v0z;
        
        // Cross product: rayDir × edge2
        float hx = rayDirY * edge2z - rayDirZ * edge2y;
        float hy = rayDirZ * edge2x - rayDirX * edge2z;
        float hz = rayDirX * edge2y - rayDirY * edge2x;
        
        // Dot product: edge1 · h
        float a = edge1x * hx + edge1y * hy + edge1z * hz;
        
        if (a > -EPSILON && a < EPSILON) return false; // Ray parallel to triangle
        
        float f = 1.0f / a;
        
        // Vector from v0 to ray origin
        float sx = rayOriginX - v0x, sy = rayOriginY - v0y, sz = rayOriginZ - v0z;
        
        // u parameter
        float u = f * (sx * hx + sy * hy + sz * hz);
        if (u < 0.0f || u > 1.0f) return false;
        
        // Cross product: s × edge1
        float qx = sy * edge1z - sz * edge1y;
        float qy = sz * edge1x - sx * edge1z;
        float qz = sx * edge1y - sy * edge1x;
        
        // v parameter
        float v = f * (rayDirX * qx + rayDirY * qy + rayDirZ * qz);
        if (v < 0.0f || u + v > 1.0f) return false;
        
        // t parameter (distance along ray)
        t = f * (edge2x * qx + edge2y * qy + edge2z * qz);
        
        return t > EPSILON; // Hit in front of ray origin
    }
}

#endif
