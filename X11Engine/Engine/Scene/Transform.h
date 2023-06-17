#pragma once

#include "Framework/Types/Matrix.h"
#include "Framework/Types/Quaternion.h"
#include "Framework/Types/Vector3.h"

struct Transform {
    Transform(matrix localMatrix)
        : position(localMatrix.Translation()),
          scale(localMatrix.Scale()),
          rotation(localMatrix.Rotation()) {}
    Transform(vector3 positions, vector3 scale, quaternion rotation)
        : position(positions), scale(scale), rotation(rotation) {}

    void CalcWorldMatrix(const matrix& parentMatrix);

    vector3 GetUp() const { return (vector3::up() * worldMatrix).normalized(); }
    vector3 GetRight() const {
        return (vector3::right() * worldMatrix).normalized();
    }
    vector3 GetForward() const {
        return (vector3::forward() * worldMatrix).normalized();
    }

    vector3 position;
    vector3 scale;
    quaternion rotation;

    matrix worldMatrix;
};