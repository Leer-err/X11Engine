#ifndef SHAPE_H
#define SHAPE_H

#include "PxMaterial.h"
#include "Transform.h"
#include "Vector3.h"
#include "geometry/PxGeometry.h"
namespace physx {
class PxShape;
};

namespace Physics {

class Shape {
    friend class StaticRigidBody;

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
    Shape(physx::PxGeometry& geometry);

    physx::PxShape* shape;
};

};  // namespace Physics

#endif  // SHAPE_H