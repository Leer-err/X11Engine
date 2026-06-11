#include "VFXWorld.h"

namespace Graphics {

VFXWorld::VFXWorld(size_t max_particle_count) : pool(max_particle_count) {}

void VFXWorld::update(float delta_time) {
    for (auto& effect : effects) {
        effect.update(delta_time);
    }
}

}  // namespace Graphics