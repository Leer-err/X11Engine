#include "Resources.h"

#include <PxPhysicsAPI.h>

namespace Physics {

Resources::Resources() {
    foundation = PxCreateFoundation(
        PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
    if (foundation == nullptr) return false;

    bool recordMemoryAllocations = true;
    physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation,
                              physx::PxTolerancesScale(),
                              recordMemoryAllocations, pvd);
}

physx::PxPhysics* Resources::getPhysics() { return get().physics; }

Resources& Resources::get() {
    static Resources instance;
    return instance;
}

};  // namespace Physics