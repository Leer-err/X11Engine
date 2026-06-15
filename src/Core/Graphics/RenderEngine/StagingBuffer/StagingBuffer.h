#pragma once

#include <vulkan/vulkan_core.h>

#include <cstddef>
#include <vector>

#include "Buffer.h"
#include "CommandBuffer.h"
#include "Device.h"


namespace Graphics {

class StagingBuffer {
    struct BufferData {
        Buffer buffer;
        size_t host_offset;

        size_t data_size;
    };

    struct TextureData {
        Texture texture;
        size_t width;
        size_t height;
        size_t host_offset;

        size_t data_size;
    };

   public:
    StagingBuffer(Device& device);

    void stageTexture(const Texture& destination, const void* data,
                      size_t data_size);
    void stageBuffer(const Buffer& destination, const void* data,
                     size_t data_size);

    void flush(const CommandBuffer& cmd);

   private:
    Device& device;

    size_t host_data_used;

    Buffer buffer;

    size_t getTexelBlockSize(VkFormat format) const;

    std::vector<BufferData> buffers;
    std::vector<TextureData> textures;
};

}  // namespace Graphics