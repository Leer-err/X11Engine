#pragma once

#include "Matrix.h"
#include "Vector3.h"

struct CameraData {
    Matrix view_projection;
    Matrix view_projection_camera_centered;
    Vector3 up;
    Vector3 right;
};