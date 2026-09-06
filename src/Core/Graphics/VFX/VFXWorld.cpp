#include "VFXWorld.h"

#include <algorithm>
#include <random>
#include <span>

#include "Effect.h"
#include "EffectDescription.h"

namespace Graphics {

VFXWorld::VFXWorld(size_t max_particle_count)
    : particles(max_particle_count), allocator(max_particle_count) {}

void VFXWorld::update(float delta_time) {
    for (auto& effect : effects) effect.update(delta_time);
    for (auto& effect : effects) effect.emit(delta_time);
}

void VFXWorld::addEffect(const EffectDescription& description) {
    effects.emplace_back(description, allocator, particles);
}

VFXWorld::ParticleBatch VFXWorld::getParticles() const {
    auto allocated_particles = allocator.getAllocated();
    auto last_live_particle = 0uLL;
    if (!allocated_particles.empty()) {
        last_live_particle = *std::ranges::max_element(allocated_particles);
    }

    return ParticleBatch{std::span(particles.data(), last_live_particle + 1),
                         allocator.getAllocated(), last_live_particle};
}

}  // namespace Graphics