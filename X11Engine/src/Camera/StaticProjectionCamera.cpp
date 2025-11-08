#include "StaticProjectionCamera.h"

#include <cmath>

#include "Matrix.h"

constexpr float PI = 3.14156535;

StaticProjectionCamera::StaticProjectionCamera(float fov, float aspect_ratio,
                                               float near_clip, float far_clip)
    : fov(fov / 180 * PI),
      aspect_ratio(aspect_ratio),
      near_clip(near_clip),
      far_clip(far_clip) {}

Matrix StaticProjectionCamera::getProjectionMatrix() const {
    return Matrix::projection(fov, aspect_ratio, far_clip, near_clip);
}