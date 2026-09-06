#pragma once

#include "Property.h"
#include "TextureHandle.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Graphics {

struct EffectDescription {
    Vector3 center;
    Vector3 extents;
    float spawn_rate;
    Property<Vector4> color;
    Property<float> size;
    Property<float> rotation;
    float particle_lifetime;
    TextureHandle texture;
};

}  // namespace Graphics