#pragma once

namespace Physics {

class Resources {
   public:
    static physx::PxPhysics* getPhysics();

   private:
    Resources();

    static Resources& get();

    physx::PxPvd* pvd;

    physx::PxFoundation* foundation;
    physx::PxPhysics* physics;
    physx::PxScene* scene;
};

};  // namespace Physics