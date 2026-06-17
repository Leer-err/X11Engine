#pragma once

#include <cstddef>

#include "Buffer.h"
#include "BufferRegistry/BufferRegistry.h"
#include "Device.h"
#include "EngineData.h"
#include "Result.h"

namespace Graphics {

class BufferBuilder {
   public:
    explicit BufferBuilder(size_t size);

    BufferBuilder& isShaderResource();
    BufferBuilder& isVertexBuffer();
    BufferBuilder& isIndexBuffer();
    BufferBuilder& isConstantBuffer();
    BufferBuilder& isDescriptorBuffer();

    BufferBuilder& isCPUWritable(bool always_mapped = false,
                                 bool is_random = false);

    BufferBuilder& isCopySource();
    BufferBuilder& isCopyDestination();

    BufferBuilder& isDeviceAddressable();

    BufferBuilder& isChained();

    Result<Buffer, BufferError> create(Device& device) const;

   private:
    VkBufferCreateInfo buffer_info;
    VmaAllocationCreateInfo alloc_info;

    bool is_chained = false;
};

}  // namespace Graphics