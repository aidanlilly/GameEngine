#include "Camera.h"
#include <cmath>
#include <algorithm>

Camera::Camera() 
    : distance_(10.0f)
    , yaw_(0.0f)
    , pitch_(0.3f)
    , panX_(0.0f)
    , panY_(0.0f)
    , panZ_(0.0f)
    , fovDegrees_(45.0f)
    , nearPlane_(0.1f)
    , farPlane_(10000.0f)
{
}

void Camera::orbit(float deltaYaw, float deltaPitch) {
    yaw_ += deltaYaw;
    pitch_ += deltaPitch;
    // Clamp pitch to avoid gimbal lock
    pitch_ = std::max(-1.5f, std::min(1.5f, pitch_));
}

void Camera::pan(float deltaX, float deltaY) {
    float panSpeed = 0.01f * distance_;
    panX_ += deltaX * panSpeed;
    panY_ += deltaY * panSpeed;
}

void Camera::zoom(float delta) {
    // Simple linear zoom with minimum distance to avoid going through floor
    distance_ -= delta * 0.5f;
    distance_ = std::max(0.5f, distance_);  // Minimum zoom distance
}

void Camera::getViewMatrix(float* out16) const {
    // Calculate camera position (orbit around target with pan offset)
    float cx = distance_ * cosf(pitch_) * sinf(yaw_);
    float cy = distance_ * sinf(pitch_);
    float cz = distance_ * cosf(pitch_) * cosf(yaw_);
    
    float targetX = panX_;
    float targetY = panY_;
    float targetZ = panZ_;
    
    float eyeX = cx + targetX;
    float eyeY = cy + targetY;
    float eyeZ = cz + targetZ;

    // Build lookAt view matrix
    // Forward vector (from eye to target)
    float fx = targetX - eyeX;
    float fy = targetY - eyeY;
    float fz = targetZ - eyeZ;
    float flen = sqrtf(fx*fx + fy*fy + fz*fz);
    fx /= flen; fy /= flen; fz /= flen;
    
    // Up vector
    float upX = 0.0f, upY = 1.0f, upZ = 0.0f;
    
    // Right = forward × up
    float sx = fy * upZ - fz * upY;
    float sy = fz * upX - fx * upZ;
    float sz = fx * upY - fy * upX;
    float slen = sqrtf(sx*sx + sy*sy + sz*sz);
    sx /= slen; sy /= slen; sz /= slen;
    
    // Recompute up = right × forward
    float ux = sy * fz - sz * fy;
    float uy = sz * fx - sx * fz;
    float uz = sx * fy - sy * fx;
    
    // Build view matrix (column-major)
    out16[0] = sx;
    out16[1] = ux;
    out16[2] = -fx;
    out16[3] = 0.0f;
    
    out16[4] = sy;
    out16[5] = uy;
    out16[6] = -fy;
    out16[7] = 0.0f;
    
    out16[8] = sz;
    out16[9] = uz;
    out16[10] = -fz;
    out16[11] = 0.0f;
    
    out16[12] = -(sx*eyeX + sy*eyeY + sz*eyeZ);
    out16[13] = -(ux*eyeX + uy*eyeY + uz*eyeZ);
    out16[14] = -(-fx*eyeX + -fy*eyeY + -fz*eyeZ);
    out16[15] = 1.0f;
}

void Camera::getProjectionMatrix(float* out16, float aspect) const {
    float fovRadians = fovDegrees_ * 3.14159265f / 180.0f;
    float f = 1.0f / std::tan(fovRadians * 0.5f);
    
    out16[0] = f / aspect;
    out16[1] = 0.0f;
    out16[2] = 0.0f;
    out16[3] = 0.0f;
    
    out16[4] = 0.0f;
    out16[5] = f;
    out16[6] = 0.0f;
    out16[7] = 0.0f;
    
    out16[8] = 0.0f;
    out16[9] = 0.0f;
    out16[10] = (farPlane_ + nearPlane_) / (nearPlane_ - farPlane_);
    out16[11] = -1.0f;
    
    out16[12] = 0.0f;
    out16[13] = 0.0f;
    out16[14] = (2.0f * farPlane_ * nearPlane_) / (nearPlane_ - farPlane_);
    out16[15] = 0.0f;
}

void Camera::moveRelative(float forward, float right, float up) {
    // Build orientation vectors from yaw/pitch
    float cy = cosf(yaw_);
    float sy = sinf(yaw_);
    float cp = cosf(pitch_);
    float sp = sinf(pitch_);

    // Forward (eye -> target) matching view forward used in getViewMatrix
    float fwdX = -sy * cp;
    float fwdY = -sp;
    float fwdZ = -cy * cp;

    // Right vector on horizontal plane (ignore pitch for strafing)
    float rightX = cy;
    float rightZ = -sy;

    // Movement speed scaled by distance for consistent feel
    float speed = std::max(0.001f, 0.02f * distance_);

    panX_ += (fwdX * forward + rightX * right) * speed;
    panY_ += (fwdY * forward + up) * speed;
    panZ_ += (fwdZ * forward + rightZ * right) * speed;
}

void Camera::getEyePosition(float& x, float& y, float& z) const {
    // Mirror the computation in getViewMatrix
    float cx = distance_ * cosf(pitch_) * sinf(yaw_);
    float cy = distance_ * sinf(pitch_);
    float cz = distance_ * cosf(pitch_) * cosf(yaw_);

    x = cx + panX_;
    y = cy + panY_;
    z = cz + panZ_;
}
