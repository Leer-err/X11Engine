#ifndef STATIC_RIGID_BODY_H
#define STATIC_RIGID_BODY_H

#include "Quaternion.h"
namespace physx {
class PxRigidStatic;
}

class StaticRigidBody {
   public:
    StaticRigidBody(physx::PxRigidStatic* body);
    StaticRigidBody(StaticRigidBody&& other);
    ~StaticRigidBody();

    physx::PxRigidStatic* get() const;

    void setTransform(const Vector3& position, const Quaternion& quaternion);

   private:
    physx::PxRigidStatic* body;
};

#endif  // STATIC_RIGID_BODY_H