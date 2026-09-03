#pragma once

#include "TextureHandle.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Graphics {

struct ParticleState {
    Vector3 position;
    Vector4 color;
    float size;
    Vector3 velocity;
    TextureHandle texture;
    float lifetime;
};

}  // namespace Graphics