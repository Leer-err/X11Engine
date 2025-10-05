#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Matrix.h"
#include "Quaternion.h"
#include "Vector3.h"
#include "Vector4.h"

struct Transform {
    Transform() {
        scale.x = 1.;
        scale.y = 1.;
        scale.z = 1.;
    }

    Vector3 position;
    Quaternion orientation;
    Vector3 scale;

    Matrix toMatrix() const {
        Matrix translation = Matrix::translation(position);

        Matrix rotation = Matrix::rotation(orientation);

        Matrix scale_matrix = Matrix::scale(scale);

        return scale_matrix * rotation * translation;
    }
};

#endif  // TRANSFORM_H