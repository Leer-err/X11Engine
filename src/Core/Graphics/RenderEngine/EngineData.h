#pragma once

#include "BufferRegistry.h"
#include "DescriptorSet.h"
#include "MeshRegistry.h"
#include "ShaderRegistry.h"
#include "StagingBuffer.h"
#include "TextureRegistry.h"

namespace Graphics {

struct EngineData {
    DescriptorSet& descriptor_set;
    ShaderRegistry& shader_registry;
    MeshRegistry& mesh_registry;
    TextureRegistry& texture_registry;
    BufferRegistry& buffer_registry;

    StagingBuffer& staging_buffer;
};

}  // namespace Graphics