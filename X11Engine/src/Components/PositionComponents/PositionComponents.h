#ifndef POSITION_COMPONENTS_H
#define POSITION_COMPONENTS_H

#include <string>

#include "Matrix.h"
#include "Quaternion.h"
#include "Vector3.h"

struct DirtyTransform {};

struct Position {
    Vector3 position;
};

struct Rotation {
    Quaternion orientation;
};

struct Scale {
    Vector3 scale;
};

struct LocalMatrix {
    Matrix matrix;
};

struct GlobalMatrix {
    Matrix matrix;

    std::string serialize() const;
};

#endif  // POSITION_COMPONENTS_H