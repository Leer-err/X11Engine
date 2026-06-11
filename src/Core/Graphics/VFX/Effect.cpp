#include "Effect.h"

namespace Graphics {

Effect::Effect(ParticlePool& pool) : emitters({Emitter(pool, 100)}) {}

void Effect::update(float delta_time) {
    for (auto& emitter : emitters) {
        emitter.update(delta_time);
    }
}

}  // namespace Graphics