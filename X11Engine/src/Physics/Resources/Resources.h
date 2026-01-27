#pragma once

namespace physx {
class PxFoundation;
class PxPhysics;
}  // namespace physx

namespace Physics {

class Resources {
   public:
    static physx::PxPhysics* getPhysics();

   private:
    Resources();

    static Resources& get();

    physx::PxFoundation* foundation;
    physx::PxPhysics* physics;
};

};  // namespace Physics