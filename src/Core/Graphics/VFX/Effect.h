#pragma once

#include <vector>

#include "EffectDescription.h"
#include "Handles.h"
#include "MeshHandle.h"
#include "Particle.h"
#include "ParticleHandle.h"
#include "ParticleHandleAllocator.h"
#include "Property.h"


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

class MeshEffect {
   public:
    MeshEffect(const MeshEffectDescription& description,
               ParticleHandleAllocator& allocator,
               std::vector<MeshParticle>& particles);

    void update(float delta_time);

    void emit(float delta_time);

   private:
    ParticleHandleAllocator& allocator;
    std::vector<MeshParticle>& particles;

    std::vector<ParticleHandle> owned_particles;

    float spawn_error = 0;

    BoxEmitter emitter;
    float lifetime;
    TextureHandle texture;
    MeshHandle mesh;
};

}  // namespace Graphics