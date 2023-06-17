#pragma once

#include "Vector2.h"
#include "Vector3.h"

struct Vertex {
    Vertex(const vector3& pos, const vector3& normal, const vector2& uv)
        : pos(pos), uv(uv), normal(normal) {}

    vector3 pos;
    vector2 uv;
    vector3 normal;
};

struct VertexSkinning : public Vertex {
    VertexSkinning(const vector3& pos, const vector3& normal, const vector2& uv)
        : Vertex(pos, normal, uv) {
        memset(&indices, 0xff, sizeof(int) * 4);
        memset(&weights, 0xff, sizeof(float) * 4);
    }

    int indices[4];
    float weights[4];
};