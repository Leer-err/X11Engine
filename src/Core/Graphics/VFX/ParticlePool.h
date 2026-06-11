#pragma once

#include <cstddef>
#include <deque>
#include <vector>

#include "ParticleHandle.h"
#include "TextureHandle.h"
#include "Vector3.h"

namespace Graphics {

struct ParticleBatchState {
    std::vector<Vector3> positions;
    std::vector<Vector3> velocities;
    std::vector<TextureHandle> textures;
    std::vector<float> lifetimes;
    std::vector<int> alive_flags;
};

class ParticlePool {
   public:
    explicit ParticlePool(size_t size);

    ParticleHandle allocate();
    void free(ParticleHandle index);

    const ParticleBatchState& getBatch() const;

   private:
    ParticleBatchState batch;
    std::deque<size_t> free_list;
};

}  // namespace Graphics