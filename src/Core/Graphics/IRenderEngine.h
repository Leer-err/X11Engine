#pragma once

#include <cstddef>

#include "Mesh.h"
#include "MeshHandle.h"
#include "RenderWorld.h"
#include "TextureHandle.h"

namespace Graphics {

class IRenderEngine {
   public:
    virtual void render() = 0;

    virtual RenderWorld& getRenderWorld() = 0;

    virtual TextureHandle addTexture(void* data, uint32_t width,
                                     uint32_t height) = 0;
    virtual MeshHandle addMesh(const ::Mesh& mesh) = 0;
};

}  // namespace Graphics