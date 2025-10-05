#include "StaticRigidBody.h"

#include <utility>

#include "PxRigidStatic.h"
#include "Vector3.h"
#include "foundation/PxQuat.h"
#include "foundation/PxTransform.h"

StaticRigidBody::StaticRigidBody(physx::PxRigidStatic* body) : body(body) {}

StaticRigidBody::StaticRigidBody(StaticRigidBody&& other) {
    this->body = nullptr;

    std::swap(other.body, this->body);
}

StaticRigidBody::~StaticRigidBody() {
    if (body == nullptr) return;

    body->release();
}

void StaticRigidBody::setTransform(const Vector3& position,
                                   const Quaternion& quaternion) {
    physx::PxVec3 px_position = {position.x, position.y, position.z};
    physx::PxQuat px_rotation = {quaternion.x, quaternion.y, quaternion.z,
                                 quaternion.w};

    body->setGlobalPose(physx::PxTransform(px_position, px_rotation));
}

physx::PxRigidStatic* StaticRigidBody::get() const { return body; }