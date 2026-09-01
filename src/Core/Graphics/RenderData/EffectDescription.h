#pragma once

#include <variant>
#include <vector>

#include "TextureHandle.h"
#include "Vector3.h"

namespace Graphics {

struct BoxSpawner {
    Vector3 center;
    Vector3 extents;
};

struct EmitterDescription {
    BoxSpawner spawner;
    float spawn_rate;
    float particle_lifetime;
    TextureHandle texture;
};

struct EffectDescription {
    std::vector<EmitterDescription> emitters;
};

}  // namespace Graphics