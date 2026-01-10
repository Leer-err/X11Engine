#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <string>

#include "Matrix.h"
#include "Quaternion.h"
#include "Vector3.h"

class Transform {
   public:
    Transform();

    void setLocalPosition(const Vector3& position);
    void setLocalOrientation(const Quaternion& orientation);
    void setLocalScale(const Vector3& scale);

    Vector3 getLocalPosition();
    Quaternion getLocalOrientation();
    Vector3 getLocalScale();

    Vector3 getPosition();
    Quaternion getOrientation();
    Vector3 getScale();

    Vector3 getForward();
    Vector3 getRight();
    Vector3 getUp();

    Matrix getLocalMatrix();
    Matrix getGlobalMatrix();

    void setParentTransformMatrix(const Matrix& global_matrix);

    std::string serialize() const;

   private:
    void updateGlobals();
    void updateLocalMatrix();

    Vector3 local_position;
    Quaternion local_orientation;
    Vector3 local_scale;

    Vector3 position;
    Quaternion orientation;
    Vector3 scale;

    Matrix local_matrix;
    Matrix global_matrix;
    Matrix parent_matrix;

    bool dirty_local_matrix;
    bool dirty_globals;
};

#endif  // TRANSFORM_H