#include "ParticlePool.h"

namespace Graphics {

ParticlePool::ParticlePool(size_t size) {
    batch.positions.resize(size);
    batch.velocities.resize(size);
    batch.alive_flags.resize(size);
    batch.lifetimes.resize(size);
    batch.textures.resize(size);

    for (size_t i = 0; i < size; i++) {
        free_list.push_back(i);
    }
}

ParticleHandle ParticlePool::allocate() {
    auto index = free_list.front();
    free_list.pop_front();

    return index;
}

void ParticlePool::free(ParticleHandle index) { free_list.push_back(index); }

const ParticleBatchState& ParticlePool::getBatch() const { return batch; }

}  // namespace Graphics