#pragma once

#include "TextureHandle.h"
#include "Vector3.h"

namespace Graphics {

struct ParticleState {
    Vector3 position;
    Vector3 velocity;
    TextureHandle texture;
    float lifetime;
};

}  // namespace Graphics