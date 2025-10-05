#ifndef PHYSICS_FACTORY_H
#define PHYSICS_FACTORY_H

#include <memory>

#include "PxPhysics.h"
#include "PxScene.h"
#include "Quaternion.h"
#include "RigidBody/DynamicRigidBody.h"
#include "RigidBody/StaticRigidBody.h"
#include "Shape/Shape.h"
#include "Vector3.h"
#include "extensions/PxDefaultCpuDispatcher.h"
#include "foundation/PxFoundation.h"

class Entity;

class PhysicsFactory {
   public:
    PhysicsFactory();

    bool init();

    void simulate(float delta_time);

    Shape createCubeShape(Vector3 half_extents, bool trigger);
    Shape createSphereShape(float radius, bool trigger);
    Shape createCapsuleShape(float half_height, float radius, bool trigger);
    Shape createPlaneShape(bool trigger);

    std::shared_ptr<DynamicRigidBody> createRigidDynamic(
        Shape shape, float density, Entity* entity = nullptr);
    std::shared_ptr<StaticRigidBody> createRigidStatic(
        Vector3 position, Quaternion quat, Shape shape,
        Entity* entity = nullptr);

   private:
    physx::PxDefaultCpuDispatcher* dispatcher;

    physx::PxPvd* pvd;

    physx::PxFoundation* foundation;
    physx::PxPhysics* physics;
    physx::PxScene* scene;
};

#endif  // PHYSICS_FACTORY_H