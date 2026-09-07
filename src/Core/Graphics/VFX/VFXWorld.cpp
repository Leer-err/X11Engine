#include "VFXWorld.h"

#include <algorithm>
#include <random>
#include <span>

#include "Effect.h"
#include "EffectDescription.h"

namespace Graphics {

VFXWorld::VFXWorld(size_t max_particle_count)
    : sprite_particles(max_particle_count),
      sprite_allocator(max_particle_count),
      mesh_particles(max_particle_count),
      mesh_allocator(max_particle_count) {}

void VFXWorld::update(float delta_time) {
    for (auto& effect : effects) effect.update(delta_time);
    for (auto& effect : mesh_effects) effect.update(delta_time);
    for (auto& effect : effects) effect.emit(delta_time);
    for (auto& effect : mesh_effects) effect.emit(delta_time);
}

void VFXWorld::addEffect(const EffectDescription& description) {
    effects.emplace_back(description, sprite_allocator, sprite_particles);
}

void VFXWorld::addMeshEffect(const MeshEffectDescription& description) {
    mesh_effects.emplace_back(description, mesh_allocator, mesh_particles);
}

VFXWorld::ParticleBatch VFXWorld::getParticles() const {
    auto allocated_particles = sprite_allocator.getAllocated();
    auto last_live_particle = 0u;
    if (!allocated_particles.empty()) {
        last_live_particle = *std::ranges::max_element(allocated_particles);
    }

    return ParticleBatch{
        std::span(sprite_particles.data(), last_live_particle + 1),
        allocated_particles, last_live_particle};
}

VFXWorld::MeshParticleBatch VFXWorld::getMeshParticles() const {
    auto allocated_particles = mesh_allocator.getAllocated();
    auto last_live_particle = 0u;
    if (!allocated_particles.empty()) {
        last_live_particle = *std::ranges::max_element(allocated_particles);
    }

    return MeshParticleBatch{
        std::span(mesh_particles.data(), last_live_particle + 1),
        allocated_particles, last_live_particle};
}

}  // namespace Graphics