#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <string>

#include "Matrix.h"
#include "Quaternion.h"
#include "Vector3.h"


class Transform {
   public:
    Transform();

    void setPosition(const Vector3& position);
    void setOrientation(const Quaternion& orientation);
    void setScale(const Vector3& scale);

    Vector3 getPosition() const;
    Quaternion getOrientation() const;
    Vector3 getScale() const;

    Vector3 getForward() const;
    Vector3 getRight() const;
    Vector3 getUp() const;

    Matrix getLocalMatrix();

    bool isDirty() const;

    std::string serialize() const;

   private:
    Vector3 position;
    Quaternion orientation;
    Vector3 scale;

    Matrix local_matrix;
    bool dirty_matrix;
};

#endif  // TRANSFORM_H