#pragma once

#include "DescriptorSet.h"
#include "MeshRegistry.h"
#include "Registries.h"
#include "ShaderRegistry.h"
#include "StagingBuffer.h"

namespace Graphics {

struct EngineData {
    DescriptorSet& descriptor_set;
    ShaderRegistry& shader_registry;
    MeshRegistry& mesh_registry;
    TextureRegistry& texture_registry;

    StagingBuffer& staging_buffer;
};

}  // namespace Graphics