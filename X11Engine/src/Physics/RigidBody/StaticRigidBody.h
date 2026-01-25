#ifndef STATIC_RIGID_BODY_H
#define STATIC_RIGID_BODY_H

#include "Actor.h"
#include "Quaternion.h"

namespace physx {
class PxRigidStatic;
}

class StaticRigidBody : public Actor {
   public:
    StaticRigidBody(physx::PxRigidStatic* body);
    StaticRigidBody(StaticRigidBody&& other);
    ~StaticRigidBody();

    physx::PxRigidStatic* get() const;

    void setTransform(const Vector3& position, const Quaternion& quaternion);

    void addShape(const Shape& shape) override;

   private:
    physx::PxRigidStatic* body;
};

#endif  // STATIC_RIGID_BODY_H