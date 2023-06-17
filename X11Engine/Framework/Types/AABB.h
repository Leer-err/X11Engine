#pragma once

#include "Vector3.h"

struct AABB {
    AABB() : center(), extents() {}
    AABB(const vector3& min, const vector3& max)
        : center((max + min) / 2), extents(max - center) {}
    AABB(const vector3& center, float extentX, float extentY, float extentZ)
        : center(center), extents(extentX, extentY, extentZ) {}

    vector3 center;
    vector3 extents;
};