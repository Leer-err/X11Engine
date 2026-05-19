#include "Camera.h"

#include "Matrix.h"

constexpr float PI = 3.14156535;

Camera Camera::create(float fov, float aspect_ratio,
                      float near_clip, float far_clip) {
    Camera camera = {};
    camera.fov = fov / 180 * PI;
    camera.aspect_ratio = aspect_ratio;
    camera.near_clip = near_clip;
    camera.far_clip = far_clip;

    return camera;
}

Matrix Camera::getProjectionMatrix() const {
    auto projection = Matrix::projection(fov, aspect_ratio, far_clip, near_clip);
    projection.m[1][1] *= -1;

    return projection;
}