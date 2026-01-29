#include "Resources.h"

#include <PxPhysicsAPI.h>

static physx::PxDefaultErrorCallback gDefaultErrorCallback;
static physx::PxDefaultAllocator gDefaultAllocatorCallback;

namespace Physics {

Resources::Resources() {
    foundation = PxCreateFoundation(
        PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
    if (foundation == nullptr) return;

    bool record_memory_allocations = true;
    physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation,
                              physx::PxTolerancesScale(),
                              record_memory_allocations, nullptr);
}

physx::PxPhysics* Resources::getPhysics() { return get().physics; }

Resources& Resources::get() {
    static Resources instance;
    return instance;
}

};  // namespace Physics