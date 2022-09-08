#pragma once

#include "Types/Matrix.h"

struct Bone {
    Bone(const matrix& offset, int parent)
        : offsetMatrix(offset), parentId(parent) {}

    matrix offsetMatrix;
    int parentId;
};