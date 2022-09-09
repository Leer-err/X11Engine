#pragma once

#include "Vector2.h"
#include "Vector3.h"

struct Vertex {
    Vertex(const vector3& pos = {}, const vector2& uv = {},
           const vector3& normal = {})
        : pos(pos), uv(uv), normal(normal) {
        memset(&boneIds, 0xff, sizeof(int) * 4);
        memset(&boneWeights, 0xff, sizeof(float) * 4);
    }

    vector3 pos;
    vector2 uv;
    vector3 normal;
    int boneIds[4];
    float boneWeights[4];
};