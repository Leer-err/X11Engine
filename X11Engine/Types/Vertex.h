#pragma once

#include "Vector2.h"
#include "Vector3.h"

struct vertex {
    vertex(const vector3& pos = {}, const vector2& uv = {},
           const vector3& normal = {})
        : pos(pos), uv(uv), normal(normal){};

    vector3 pos;
    vector2 uv;
    vector3 normal;
};