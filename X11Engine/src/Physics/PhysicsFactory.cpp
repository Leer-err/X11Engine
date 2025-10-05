#include "PhysicsFactory.h"

#include <PxPhysicsAPI.h>

#include <memory>
#include <vector>

#include "Entity.h"
#include "PhysicsComponents.h"
#include "PxMaterial.h"
#include "PxRigidStatic.h"
#include "PxSceneDesc.h"
#include "Quaternion.h"
#include "RigidBody/DynamicRigidBody.h"
#include "RigidBody/StaticRigidBody.h"
#include "Shape/Shape.h"
#include "Vector3.h"
#include "foundation/PxQuat.h"
#include "foundation/PxVec3.h"
#include "geometry/PxCapsuleGeometry.h"

physx::PxMaterial* material;

static physx::PxDefaultErrorCallback gDefaultErrorCallback;
static physx::PxDefaultAllocator gDefaultAllocatorCallback;

class CollisionCallback : public physx::PxSimulationEventCallback {
    void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override {
        for (physx::PxU32 i = 0; i < count; i++) {
            if (pairs[i].status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) {
                Entity* trigger =
                    reinterpret_cast<Entity*>(pairs[i].triggerActor->userData);
                Entity* entity =
                    reinterpret_cast<Entity*>(pairs[i].otherActor->userData);
                if (trigger == nullptr || entity == nullptr) return;

                const Collisions* collisions = trigger->get<Collisions>();
                if (collisions == nullptr) {
                    trigger->set<Collisions>({{*entity}});
                    return;
                }

                auto new_collisions = std::move(*collisions);
                new_collisions.collision_list.push_back(*entity);
                trigger->set<Collisions>({std::move(new_collisions)});
            }
        }
    }

    // Other unused callbacks
    void onConstraintBreak(physx::PxConstraintInfo*, physx::PxU32) override {}
    void onWake(physx::PxActor**, physx::PxU32) override {}
    void onSleep(physx::PxActor**, physx::PxU32) override {}
    void onContact(const physx::PxContactPairHeader&,
                   const physx::PxContactPair*, physx::PxU32) override {}
    void onAdvance(const physx::PxRigidBody* const*, const physx::PxTransform*,
                   const physx::PxU32) override {}
};

CollisionCallback gCollisionCallback;

PhysicsFactory::PhysicsFactory() {}

physx::PxRigidDynamic* sphere;

bool PhysicsFactory::init() {
    foundation = PxCreateFoundation(
        PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
    if (foundation == nullptr) return false;

    pvd = PxCreatePvd(*foundation);
    physx::PxPvdTransport* transport =
        physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
    pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

    bool recordMemoryAllocations = true;
    physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation,
                              physx::PxTolerancesScale(),
                              recordMemoryAllocations, pvd);
    if (physics == nullptr) return false;

    dispatcher = physx::PxDefaultCpuDispatcherCreate(2);

    physx::PxSceneDesc sceneDesc(physics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.cpuDispatcher = dispatcher;
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
    sceneDesc.simulationEventCallback =
        &gCollisionCallback;  // Register callback
    scene = physics->createScene(sceneDesc);
    if (scene == nullptr) return false;

    material = physics->createMaterial(0.5f, 0.5f, 0.1f);

    return true;
}

Shape PhysicsFactory::createPlaneShape(bool trigger) {
    physx::PxShape* shape =
        physics->createShape(physx::PxPlaneGeometry(), *material);

    auto result = Shape(shape);
    shape->release();

    if (trigger) {
        result.setTrigger();
    } else {
        result.setPhysical();
    }

    return result;
}

Shape PhysicsFactory::createCubeShape(Vector3 half_extents, bool trigger) {
    physx::PxShape* shape = physics->createShape(
        physx::PxBoxGeometry(half_extents.x, half_extents.y, half_extents.z),
        *material);

    auto result = Shape(shape);
    shape->release();

    if (trigger) {
        result.setTrigger();
    } else {
        result.setPhysical();
    }

    return result;
}

Shape PhysicsFactory::createCapsuleShape(float half_height, float radius,
                                         bool trigger) {
    physx::PxShape* shape = physics->createShape(
        physx::PxCapsuleGeometry(radius, half_height), *material);
    shape->setLocalPose(physx::PxTransform(
        physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0, 0, 1))));

    auto result = Shape(shape);
    shape->release();

    if (trigger) {
        result.setTrigger();
    } else {
        result.setPhysical();
    }

    return result;
}

Shape PhysicsFactory::createSphereShape(float radius, bool trigger) {
    physx::PxShape* shape =
        physics->createShape(physx::PxSphereGeometry(radius), *material);

    auto result = Shape(shape);

    shape->release();

    if (trigger) {
        result.setTrigger();
    } else {
        result.setPhysical();
    }

    return result;
}

std::shared_ptr<DynamicRigidBody> PhysicsFactory::createRigidDynamic(
    Shape shape, float density, Entity* entity) {
    physx::PxTransform projectile_transform((physx::PxVec3()));
    physx::PxRigidDynamic* actor =
        physics->createRigidDynamic(projectile_transform);

    actor->userData = entity;
    actor->attachShape(*shape.get());
    physx::PxRigidBodyExt::updateMassAndInertia(*actor, density);
    scene->addActor(*actor);

    return std::make_shared<DynamicRigidBody>(actor);
}

std::shared_ptr<StaticRigidBody> PhysicsFactory::createRigidStatic(
    Vector3 position, Quaternion quat, Shape shape, Entity* entity) {
    physx::PxTransform transform(
        physx::PxVec3(position.x, position.y, position.z),
        physx::PxQuat(quat.x, quat.y, quat.z, quat.w));
    physx::PxRigidStatic* actor = physics->createRigidStatic(transform);

    actor->userData = entity;
    actor->attachShape(*shape.get());
    scene->addActor(*actor);

    return std::make_shared<StaticRigidBody>(actor);
}

void PhysicsFactory::simulate(float delta_time) {
    scene->simulate(delta_time);
    scene->fetchResults(true);
}