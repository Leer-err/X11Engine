#pragma once

#include <cstddef>
#include <vector>

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

    size_t allocate();
    void free(size_t index);

   private:
    ParticleBatchState batch;
};

}  // namespace Graphics