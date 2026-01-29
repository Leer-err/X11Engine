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
    enum class Lock {
        LockRotationX,
        LockRotationY,
        LockRotationZ,
        LockRotationXY,
        LockRotationXZ,
        LockRotationYZ,
        LockRotationXYZ
    };

    DynamicRigidBody();
    DynamicRigidBody(physx::PxRigidDynamic* body);
    ~DynamicRigidBody();

    Vector3 getPosition() const;
    Quaternion getRotation() const;

    Vector3 getVelocity() const;

    void setVelocity(const Vector3& velocity);
    void setTransform(const Vector3& position, const Quaternion& quaternion);
    void setGravity(bool gravity);
    void setCCD(bool ccd);
    void lock(Lock lock);

    physx::PxRigidDynamic* get() const;

   private:
    physx::PxRigidDynamic* body;
};

};  // namespace Physics

#endif  // DYNAMIC_RIGID_BODY_H