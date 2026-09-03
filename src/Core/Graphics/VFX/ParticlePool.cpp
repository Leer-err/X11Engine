#include "ParticlePool.h"

#include <optional>

namespace Graphics {

ParticleHandleAllocator::ParticleHandleAllocator(size_t size) {
    for (size_t i = 0; i < size; i++) {
        free_list.push_back(i);
    }
}

std::optional<ParticleHandle> ParticleHandleAllocator::allocate() {
    if (free_list.empty()) return std::nullopt;

    auto index = free_list.front();
    free_list.pop_front();

    return index;
}

void ParticleHandleAllocator::free(ParticleHandle index) {
    free_list.push_back(index);
}

ParticlePool::ParticlePool(size_t size) : allocator(size) {
    batch.positions.resize(size);
    batch.colors.resize(size);
    batch.sizes.resize(size);
    batch.velocities.resize(size);
    batch.alive_flags.resize(size);
    batch.lifetimes.resize(size);
    batch.textures.resize(size);
}

void ParticlePool::addParticle(const ParticleState& state) {
    auto particle_opt = allocator.allocate();
    if (!particle_opt.has_value()) return;

    auto particle = *particle_opt;

    batch.positions[particle] = state.position;
    batch.colors[particle] = state.color;
    batch.sizes[particle] = state.size;
    batch.velocities[particle] = state.velocity;
    batch.textures[particle] = state.texture;
    batch.lifetimes[particle] = state.lifetime;
    batch.alive_flags[particle] = true;
}

void ParticlePool::update(float delta_time) {
    for (ParticleHandle i = 0; i < batch.positions.size(); i++) {
        if (batch.alive_flags[i] == false) continue;

        auto& lifetime = batch.lifetimes[i];
        lifetime -= delta_time;
        if (lifetime < 0) {
            batch.alive_flags[i] = false;
            allocator.free(i);
            continue;
        }

        auto& position = batch.positions[i];
        const auto& velocity = batch.velocities[i];
        position = position + velocity * delta_time;
    }
}

const ParticleBatchState& ParticlePool::getParticleBatch() const {
    return batch;
}

}  // namespace Graphics