#pragma once

#include <vector>

#include "Effect.h"
#include "EffectDescription.h"
#include "ParticlePool.h"

namespace Graphics {

class VFXWorld {
   public:
    explicit VFXWorld(size_t max_particle_count);

    void addEffect(const EffectDescription& description);

    void update(float delta_time);

    const ParticleBatchState& getParticles() const;

   private:
    void emitParticles(Emitter& emitter, float delta_time);

    std::vector<Effect> effects;
    ParticlePool pool;
};

}  // namespace Graphics