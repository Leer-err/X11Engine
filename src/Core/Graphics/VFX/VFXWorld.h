#pragma once

#include <span>
#include <vector>

#include "Effect.h"
#include "EffectDescription.h"
#include "ParticleHandle.h"
#include "ParticlePool.h"

namespace Graphics {

class VFXWorld {
   public:
    struct ParticleBatch {
        std::span<const Particle> particles;
        std::set<ParticleHandle> alive;
        size_t last_live_particle;
    };

    explicit VFXWorld(size_t max_particle_count);

    void addEffect(const EffectDescription& description);

    void update(float delta_time);

    ParticleBatch getParticles() const;

   private:
    std::vector<Effect> effects;
    std::vector<Particle> particles;
    ParticleHandleAllocator allocator;
};

}  // namespace Graphics