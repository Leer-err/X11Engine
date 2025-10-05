#ifndef VERTEX_H
#define VERTEX_H

#include "Vector2.h"
#include "Vector3.h"

struct Vertex {
    Vector3 position;
    Vector3 normal;
    Vector2 uv;

    int bone_ids[4];
    float bone_weights[4];
};

#endif  // VERTEX_H