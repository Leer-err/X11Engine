#pragma once

#include "EffectDescription.h"
#include "TextureHandle.h"
#include "Vector4.h"

namespace Graphics {

struct Emitter {
    explicit Emitter(const EmitterDescription& description);

    BoxSpawner spawner;
    float should_spawn = 0;
    Vector4 color;
    float size;
    float spawn_rate;
    float particle_lifetime;
    TextureHandle texture;
};

}  // namespace Graphics