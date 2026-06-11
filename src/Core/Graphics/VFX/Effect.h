#pragma once

#include <vector>

#include "Emitter.h"
#include "ParticlePool.h"

namespace Graphics {

class Effect {
   public:
    Effect(ParticlePool& pool);

    void update(float delta_time);

   private:
    std::vector<Emitter> emitters;
};

}  // namespace Graphics