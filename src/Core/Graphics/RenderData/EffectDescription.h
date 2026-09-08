#pragma once

#include "Handles.h"
#include "MeshHandle.h"
#include "Property.h"
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

struct MeshEffectDescription {
    Vector3 center;
    Vector3 extents;
    float spawn_rate;
    float particle_lifetime;
    MeshHandle mesh;
    TextureHandle texture;
};

}  // namespace Graphics