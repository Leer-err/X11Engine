#pragma once

#include <vector>

#include "ParticleHandle.h"
#include "ParticlePool.h"

namespace Graphics {

class Emitter {
   public:
    Emitter(ParticlePool& pool, size_t particle_count);

    void update(float delta_time);

   private:
    ParticlePool& pool;
    std::vector<ParticleHandle> particles;
};

}  // namespace Graphics