#pragma once

#include <cstddef>

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
    virtual MeshHandle addMesh(void* vertex_data, size_t vertex_data_size,
                               void* index_data, size_t index_data_size) = 0;
};

}  // namespace Graphics