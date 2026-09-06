#pragma once

#include <vector>

#include "EffectDescription.h"
#include "Particle.h"
#include "ParticleHandle.h"
#include "ParticlePool.h"
#include "Property.h"
#include "TextureHandle.h"

namespace Graphics {

struct BoxEmitter {
    Vector3 center;
    Vector3 extents;
    float spawn_rate;
};

class Effect {
   public:
    Effect(const EffectDescription& description,
           ParticleHandleAllocator& allocator,
           std::vector<Particle>& particles);

    void update(float delta_time);

    void emit(float delta_time);

   private:
    ParticleHandleAllocator& allocator;
    std::vector<Particle>& particles;

    std::vector<ParticleHandle> owned_particles;

    float spawn_error = 0;

    BoxEmitter emitter;
    Property<Vector4> color;
    Property<float> size;
    Property<float> rotation;
    float lifetime;
    TextureHandle texture;
};

}  // namespace Graphics