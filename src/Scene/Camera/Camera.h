#ifndef STATIC_PROJECTION_CAMERA_H
#define STATIC_PROJECTION_CAMERA_H

#include "Matrix.h"

struct Camera {
    static Camera create(float fov, float aspect_ratio, float near_clip,
                           float far_clip);

    Matrix getProjectionMatrix() const;

    float fov;
    float aspect_ratio;
    float near_clip;
    float far_clip;
};

#endif  // STATIC_PROJECTION_CAMERA_H