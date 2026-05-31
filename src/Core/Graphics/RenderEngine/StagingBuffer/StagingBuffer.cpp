#include "StagingBuffer.h"

#include <vulkan/vulkan.h>

#include <cstring>
#include <vector>

#include "BufferBuilder.h"
#include "Device.h"

namespace Graphics {

constexpr size_t BUFFER_SIZE = 64 * 1024 * 1024;

StagingBuffer::StagingBuffer(Device& device)
    : device(device), host_data_used(0) {
    buffer = BufferBuilder(BUFFER_SIZE)
                 .isCopySource()
                 .isDeviceAddressable()
                 .isCPUWritable(true, true)
                 .create(device)
                 .getResult();
}

void StagingBuffer::stageTexture(const Texture& destination, const void* data,
                                 size_t data_size) {
    memcpy(reinterpret_cast<char*>(buffer.mapped_address) + host_data_used,
           data, data_size);

    TextureData image_data = {};
    image_data.texture = destination;
    image_data.data_size = data_size;
    image_data.host_offset = host_data_used;
    textures.push_back(image_data);

    host_data_used += data_size;
}

void StagingBuffer::stageBuffer(const Buffer& destination, const void* data,
                                size_t data_size) {
    memcpy(reinterpret_cast<char*>(buffer.mapped_address) + host_data_used,
           data, data_size);

    BufferData buffer_data = {};
    buffer_data.buffer = destination;
    buffer_data.data_size = data_size;
    buffer_data.host_offset = host_data_used;
    buffers.push_back(buffer_data);

    host_data_used += data_size;
}

void StagingBuffer::flush(const CommandBuffer& cmd) {
    if (host_data_used == 0) return;

    std::vector<VkBufferMemoryBarrier2> buffer_barriers;
    std::vector<VkImageMemoryBarrier2> image_barriers;

    buffer_barriers.push_back(buffer.createBarrier(
        VK_PIPELINE_STAGE_2_HOST_BIT, VK_ACCESS_2_HOST_WRITE_BIT,
        VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT, VK_ACCESS_2_TRANSFER_READ_BIT));

    for (auto& buffer_data : buffers) {
        buffer_barriers.push_back(buffer_data.buffer.createBarrier(
            VK_PIPELINE_STAGE_2_NONE, VK_ACCESS_2_NONE,
            VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT,
            VK_ACCESS_2_TRANSFER_WRITE_BIT));
    }

    for (auto& image_data : textures) {
        image_barriers.push_back(image_data.texture.createBarrier(
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_2_NONE,
            VK_ACCESS_2_NONE, VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT,
            VK_ACCESS_2_TRANSFER_WRITE_BIT));
    }

    VkBufferMemoryBarrier2* buffer_barriers_ptr = nullptr;
    VkImageMemoryBarrier2* image_barriers_ptr = nullptr;

    if (buffer_barriers.empty() == false) {
        buffer_barriers_ptr = buffer_barriers.data();
    }
    if (image_barriers.empty() == false) {
        image_barriers_ptr = image_barriers.data();
    }

    cmd.barrier(image_barriers_ptr, image_barriers.size(), buffer_barriers_ptr,
                buffer_barriers.size());

    for (auto& buffer_data : buffers) {
        VkBufferCopy copy = {};
        copy.srcOffset = buffer_data.host_offset;
        copy.size = buffer_data.data_size;

        vkCmdCopyBuffer(cmd.buffer, buffer.buffer, buffer_data.buffer.buffer, 1,
                        &copy);
    }

    for (auto& image_data : textures) {
        auto state = image_data.texture.getState();

        VkBufferImageCopy copy = {};
        copy.bufferOffset = image_data.host_offset;
        copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copy.imageSubresource.layerCount = 1;
        copy.imageExtent.width = state.width;
        copy.imageExtent.height = state.height;
        copy.imageExtent.depth = 1;

        vkCmdCopyBufferToImage(cmd.buffer, buffer.buffer, state.texture,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
    }

    textures.clear();
    buffers.clear();
    host_data_used = 0;
}

}  // namespace Graphics