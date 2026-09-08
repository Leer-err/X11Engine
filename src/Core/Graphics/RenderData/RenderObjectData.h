#pragma once

#include "Handles.h"
#include "MeshHandle.h"
#include "Vector3.h"

struct RenderObjectData {
    Vector3 position;

    Graphics::TextureHandle albedo;
    MeshHandle mesh;
};