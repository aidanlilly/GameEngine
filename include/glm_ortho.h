#pragma once
// Minimal orthographic matrix generator (column-major, OpenGL style)
#include <cmath>

inline void ortho(float left, float right, float bottom, float top, float near, float far, float* out)
{
    for (int i = 0; i < 16; ++i) out[i] = 0.0f;
    out[0] = 2.0f / (right - left);
    out[5] = 2.0f / (top - bottom);
    out[10] = -2.0f / (far - near);
    out[12] = -(right + left) / (right - left);
    out[13] = -(top + bottom) / (top - bottom);
    out[14] = -(far + near) / (far - near);
    out[15] = 1.0f;
}
