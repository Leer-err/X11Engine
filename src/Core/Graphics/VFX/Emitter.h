#pragma once

#include <vector>

#include "ParticleHandle.h"
#include "ParticlePool.h"

namespace Graphics {

class Emitter {
   public:
    void update();

   private:
    ParticlePool& pool;
    std::vector<ParticleHandle> particles;
};

}  // namespace Graphics