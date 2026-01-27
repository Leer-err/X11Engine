#include "Resources.h"

namespace Physics {

Resources::Resources() {}

physx::PxPhysics* Resources::getPhysics() { return get().physics; }

Resources& Resources::get() {
    static Resources instance;
    return instance;
}

};  // namespace Physics