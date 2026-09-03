#include "VFXWorld.h"

#include <random>

#include "Effect.h"
#include "EffectDescription.h"
#include "Emitter.h"

namespace Graphics {

VFXWorld::VFXWorld(size_t max_particle_count) : pool(max_particle_count) {}

void VFXWorld::update(float delta_time) {
    pool.update(delta_time);

    for (auto& effect : effects) {
        for (auto& emitter : effect.emitters)
            emitParticles(emitter, delta_time);
    }
}

void VFXWorld::emitParticles(Emitter& emitter, float delta_time) {
    emitter.should_spawn += delta_time * emitter.spawn_rate;
    auto particle_count = static_cast<int>(emitter.should_spawn);
    emitter.should_spawn -= particle_count;

    std::random_device rd;
    std::mt19937 gen(rd());

    Vector3 min_point = emitter.spawner.center - emitter.spawner.extents;
    Vector3 max_point = emitter.spawner.center + emitter.spawner.extents;
    std::uniform_real_distribution<float> x_dist(min_point.x, max_point.x);
    std::uniform_real_distribution<float> y_dist(min_point.y, max_point.y);
    std::uniform_real_distribution<float> z_dist(min_point.z, max_point.z);
    std::uniform_real_distribution<float> vel_dist(-0.1, 0.1);

    for (int i = 0; i < particle_count; i++) {
        float x = x_dist(gen);
        float y = y_dist(gen);
        float z = z_dist(gen);

        ParticleState state = {};
        state.texture = emitter.texture;
        state.size = emitter.size;
        state.position = Vector3(x, y, z);
        state.color = emitter.color;
        state.lifetime = emitter.particle_lifetime;
        // state.velocity = Vector3(vel_dist(gen), 1, vel_dist(gen));

        pool.addParticle(state);
    }
}

void VFXWorld::addEffect(const EffectDescription& description) {
    Effect effect;
    for (const auto& emitter_description : description.emitters)
        effect.emitters.push_back(Emitter(emitter_description));

    effects.push_back(effect);
}

const ParticleBatchState& VFXWorld::getParticles() const {
    return pool.getParticleBatch();
}

}  // namespace Graphics