#ifndef DYNAMIC_RIGID_BODY_H
#define DYNAMIC_RIGID_BODY_H

#include "Actor.h"
#include "Quaternion.h"
#include "Vector3.h"

namespace physx {
class PxRigidDynamic;
};

namespace Physics {

class DynamicRigidBody : public Actor {
   public:
    enum Lock {
        LockRotationX = (1 << 0),
        LockRotationY = (1 << 1),
        LockRotationZ = (1 << 2),
    };

    DynamicRigidBody(physx::PxRigidDynamic* body);
    DynamicRigidBody(DynamicRigidBody&& other);
    ~DynamicRigidBody();

    Vector3 getPosition() const;
    Quaternion getRotation() const;

    Vector3 getVelocity() const;

    void setVelocity(const Vector3& velocity);
    void setTransform(const Vector3& position, const Quaternion& quaternion);
    void setGravity(bool gravity);
    void setCCD(bool ccd);
    void lock(uint32_t lock);

    physx::PxRigidDynamic* get() const;

   private:
    physx::PxRigidDynamic* body;
};

};  // namespace Physics

#endif  // DYNAMIC_RIGID_BODY_H