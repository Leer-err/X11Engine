#pragma once

#include <span>
#include <vector>

#include "Effect.h"
#include "EffectDescription.h"
#include "ParticleHandle.h"

namespace Graphics {

class VFXWorld {
   public:
    struct ParticleBatch {
        std::span<const Particle> particles;
        std::set<ParticleHandle> alive;
        ParticleHandle last_live_particle;
    };
    struct MeshParticleBatch {
        std::span<const MeshParticle> particles;
        std::set<ParticleHandle> alive;
        ParticleHandle last_live_particle;
    };

    explicit VFXWorld(size_t max_particle_count);

    void addEffect(const EffectDescription& description);
    void addMeshEffect(const MeshEffectDescription& description);

    void update(float delta_time);

    ParticleBatch getParticles() const;
    MeshParticleBatch getMeshParticles() const;

   private:
    std::vector<Effect> effects;
    std::vector<Particle> sprite_particles;
    ParticleHandleAllocator sprite_allocator;

    std::vector<MeshEffect> mesh_effects;
    std::vector<MeshParticle> mesh_particles;
    ParticleHandleAllocator mesh_allocator;
};

}  // namespace Graphics