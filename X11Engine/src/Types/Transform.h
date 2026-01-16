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

    Vector3 getPosition();
    Quaternion getOrientation();
    Vector3 getScale();

    Vector3 getForward();
    Vector3 getRight();
    Vector3 getUp();

    void markClean();
    void markDirty();

    bool isDirty() const;

    std::string serialize() const;

   private:
    Vector3 position;
    Quaternion orientation;
    Vector3 scale;

    bool is_dirty;
};

#endif  // TRANSFORM_H