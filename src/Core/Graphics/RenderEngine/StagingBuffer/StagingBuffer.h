#pragma once

#include <vulkan/vulkan.h>

#include <cstddef>
#include <vector>

#include "Buffer.h"
#include "CommandBuffer.h"
#include "Device.h"
#include "Handles.h"
#include "ResourceRegistry.h"

namespace Graphics {

class StagingBuffer {
    struct BufferData {
        Buffer buffer;
        size_t host_offset;

        size_t data_size;
    };

    struct TextureData {
        TextureHandle texture;
        size_t width;
        size_t height;
        size_t host_offset;

        size_t data_size;
    };

   public:
    StagingBuffer(Device& device);

    void stageTexture(TextureHandle destination, const void* data,
                      size_t data_size);
    void stageBuffer(const Buffer& destination, const void* data,
                     size_t data_size);

    void flush(const CommandBuffer& cmd);

   private:
    static size_t getTexelBlockSize(VkFormat format);
    static Buffer createBuffer(Device& device);

    Device& device;

    size_t host_data_used;

    Buffer buffer;

    std::vector<BufferData> buffers;
    std::vector<TextureData> textures;
};

}  // namespace Graphics