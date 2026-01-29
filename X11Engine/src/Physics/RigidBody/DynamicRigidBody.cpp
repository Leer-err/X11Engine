#include "DynamicRigidBody.h"

#include <PxRigidDynamic.h>
#include <foundation/PxTransform.h>

#include "Quaternion.h"

namespace Physics {

DynamicRigidBody::DynamicRigidBody() : body(nullptr) {}

DynamicRigidBody::DynamicRigidBody(physx::PxRigidDynamic* body) : body(body) {}

DynamicRigidBody::~DynamicRigidBody() {
    if (body == nullptr) return;

    body->release();
}

Vector3 DynamicRigidBody::getPosition() const {
    physx::PxTransform transform = body->getGlobalPose();

    auto pos = transform.p;

    return {pos.x, pos.y, pos.z};
}

Quaternion DynamicRigidBody::getRotation() const {
    physx::PxTransform transform = body->getGlobalPose();

    auto rot = transform.q;

    return {rot.w, rot.x, rot.y, rot.z};
}

Vector3 DynamicRigidBody::getVelocity() const {
    physx::PxVec3 velocity = body->getLinearVelocity();
    return {velocity.x, velocity.y, velocity.z};
}

void DynamicRigidBody::setVelocity(const Vector3& velocity) {
    body->setLinearVelocity({velocity.x, velocity.y, velocity.z});
}

void DynamicRigidBody::setTransform(const Vector3& position,
                                    const Quaternion& quaternion) {
    physx::PxVec3 px_position = {position.x, position.y, position.z};
    physx::PxQuat px_rotation = {quaternion.x, quaternion.y, quaternion.z,
                                 quaternion.w};

    body->setGlobalPose(physx::PxTransform(px_position, px_rotation));
}

void DynamicRigidBody::setCCD(bool ccd) {
    body->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, true);
}

void DynamicRigidBody::setGravity(bool gravity) {
    body->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !gravity);
}

void DynamicRigidBody::lock(Lock lock) {
    if (lock == Lock::LockRotationX || lock == Lock::LockRotationXY ||
        lock == Lock::LockRotationXYZ) {
        body->setRigidDynamicLockFlag(
            physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
    }

    if (lock == Lock::LockRotationY || lock == Lock::LockRotationXY ||
        lock == Lock::LockRotationXYZ) {
        body->setRigidDynamicLockFlag(
            physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
    }

    if (lock == Lock::LockRotationZ || lock == Lock::LockRotationYZ ||
        lock == Lock::LockRotationXYZ) {
        body->setRigidDynamicLockFlag(
            physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);
    }
}

void DynamicRigidBody::addShape(const Shape& shape) {
    body->attachShape(*shape.get());
}

physx::PxRigidDynamic* DynamicRigidBody::get() const { return body; }

};  // namespace Physics