#ifndef SHAPE_H
#define SHAPE_H

#include <geometry/PxGeometry.h>

#include "Transform.h"
#include "Vector3.h"

namespace physx {
class PxShape;
};

namespace Physics {

class Shape {
    friend class StaticRigidBody;
    friend class DynamicRigidBody;

   public:
    Shape();
    Shape(const Shape& shape);
    ~Shape();

    static Shape sphere(float radius);
    static Shape box(Vector3 half_extents);
    static Shape capsule(float radius, float half_height);
    static Shape plane();

    void isTrigger();
    void isPhysical();

    void setTransform(Transform transform);

   protected:
    physx::PxShape* get() const;

   private:
    Shape(const physx::PxGeometry& geometry);

    physx::PxShape* shape;
};

};  // namespace Physics

#endif  // SHAPE_H