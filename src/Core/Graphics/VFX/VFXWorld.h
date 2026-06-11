#pragma once

#include <vector>

#include "Effect.h"

namespace Graphics {

class VFXWorld {
   public:
    explicit VFXWorld(size_t max_particle_count);

    void update() const;

   private:
    std::vector<Effect> effects;
};

}  // namespace Graphics