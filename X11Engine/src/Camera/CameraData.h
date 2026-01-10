#pragma once

#include "Matrix.h"

struct CameraData {
    Matrix view_projection;
    Matrix inverse_view_projection;
};