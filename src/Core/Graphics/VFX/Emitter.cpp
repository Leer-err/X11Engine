#include "Emitter.h"

namespace Graphics {

Emitter::Emitter(ParticlePool& pool, size_t particle_count)
    : pool(pool), particles(particle_count) {}

void Emitter::update(float delta_time) {
    for (const auto& particle : particles) {
    }
}

}  // namespace Graphics