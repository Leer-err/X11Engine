#include "Types/Matrix.h"
#include "Types/Quaternion.h"
#include "Types/Vector3.h"

struct Transform {
    Transform(matrix localMatrix)
        : position(localMatrix.Translation()),
          scale(localMatrix.Scale()),
          rotation(localMatrix.Rotation()) {}
    Transform(vector3 positions, vector3 scale, quaternion rotation)
        : position(positions), scale(scale), rotation(rotation) {}

    void CalcWorldMatrix(const matrix& parentMatrix);

    vector3 GetUp() const { return (LOCAL_UP * worldMatrix).normalized(); }
    vector3 GetRight() const {
        return (LOCAL_RIGHT * worldMatrix).normalized();
    }
    vector3 GetForward() const {
        return (LOCAL_FORWARD * worldMatrix).normalized();
    }

    vector3 position;
    vector3 scale;
    quaternion rotation;

    matrix worldMatrix;
};