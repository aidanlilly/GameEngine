#ifndef CAMERA_H
#define CAMERA_H

class Camera {
public:
    Camera();
    
    // Camera control
    void orbit(float deltaYaw, float deltaPitch);
    void pan(float deltaX, float deltaY);
    void zoom(float delta);
    // Move target relative to camera orientation (forward/right) and world up
    void moveRelative(float forward, float right, float up);
    // Get computed camera world position (eye)
    void getEyePosition(float& x, float& y, float& z) const;
    
    // Getters
    void getViewMatrix(float* out16) const;
    void getProjectionMatrix(float* out16, float aspect) const;
    
    // Configuration
    void setFov(float fovDegrees) { fovDegrees_ = fovDegrees; }
    void setClipPlanes(float nearPlane, float farPlane) {
        nearPlane_ = nearPlane;
        farPlane_ = farPlane;
    }
    
    // Camera state
    float getDistance() const { return distance_; }
    float getYaw() const { return yaw_; }
    float getPitch() const { return pitch_; }
    
private:
    // Orbit camera parameters
    float distance_;
    float yaw_;
    float pitch_;
    float panX_;
    float panY_;
    float panZ_;
    
    // Projection parameters
    float fovDegrees_;
    float nearPlane_;
    float farPlane_;
};

#endif
