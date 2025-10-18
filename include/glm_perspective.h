#ifndef GLM_PERSPECTIVE_H
#define GLM_PERSPECTIVE_H

#include <cmath>
// Simple perspective matrix (right-handed, column-major)
// fovy in radians, aspect = w/h, zNear > 0, zFar > zNear
inline void perspective(float fovy, float aspect, float zNear, float zFar, float* out16) {
    float f = 1.0f / std::tan(fovy * 0.5f);
    out16[0] = f / aspect; out16[1] = 0; out16[2] = 0; out16[3] = 0;
    out16[4] = 0; out16[5] = f; out16[6] = 0; out16[7] = 0;
    out16[8] = 0; out16[9] = 0; out16[10] = (zFar + zNear) / (zNear - zFar); out16[11] = -1;
    out16[12] = 0; out16[13] = 0; out16[14] = (2 * zFar * zNear) / (zNear - zFar); out16[15] = 0;
}

#endif
