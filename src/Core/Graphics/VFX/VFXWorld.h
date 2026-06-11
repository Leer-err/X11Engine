#pragma once

#include <vector>

#include "Effect.h"
#include "ParticlePool.h"

namespace Graphics {

class VFXWorld {
   public:
    explicit VFXWorld(size_t max_particle_count);

    void update(float delta_time);

   private:
    std::vector<Effect> effects;
    ParticlePool pool;
};

}  // namespace Graphics