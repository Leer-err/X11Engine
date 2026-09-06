#pragma once

#include <cstddef>
#include <deque>
#include <optional>
#include <set>
#include <vector>

#include "ParticleHandle.h"
#include "ParticleState.h"
#include "TextureHandle.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Graphics {

class ParticleHandleAllocator {
   public:
    explicit ParticleHandleAllocator(size_t size);

    std::optional<ParticleHandle> allocate();
    void free(ParticleHandle index);

    std::set<size_t> getAllocated() const;

   private:
    std::set<size_t> free_list;
    std::set<size_t> allocated_list;
};

struct ParticleBatchState {
    std::vector<Vector3> positions;
    std::vector<Vector4> colors;
    std::vector<float> sizes;
    std::vector<Vector3> velocities;
    std::vector<TextureHandle> textures;
    std::vector<float> lifetimes;
    std::vector<int> alive_flags;
};

class ParticlePool {
   public:
    explicit ParticlePool(size_t max_particle_count);

    void addParticle(const ParticleState& state);

    void update(float delta_time);

    const ParticleBatchState& getParticleBatch() const;

   private:
    ParticleBatchState batch;
    ParticleHandleAllocator allocator;
};

}  // namespace Graphics