#pragma once

#include "MeshHandle.h"
#include "TextureHandle.h"
#include "Vector3.h"

struct OpaqueRenderObjectData {
    Vector3 position;

    TextureHandle albedo;
    MeshHandle mesh;
};