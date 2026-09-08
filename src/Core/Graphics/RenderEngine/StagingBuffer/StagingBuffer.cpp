#include "StagingBuffer.h"

#include <vulkan/vulkan.h>

#include <cstring>
#include <vector>

#include "Buffer.h"
#include "BufferBuilder.h"
#include "Device.h"
#include "Texture.h"

namespace Graphics {

constexpr size_t BUFFER_SIZE = 128 * 1024 * 1024;

StagingBuffer::StagingBuffer(Device& device)
    : device(device), host_data_used(0), buffer(createBuffer(device)) {}

void StagingBuffer::stageTexture(TextureHandle destination, const void* data,
                                 size_t data_size) {
    auto format = destination->getFormat();
    size_t alignment = getTexelBlockSize(format);
    size_t aligned_host_offset =
        (host_data_used + alignment - 1) & ~(alignment - 1);

    memcpy(buffer.getHostAddress() + aligned_host_offset, data, data_size);

    TextureData image_data = {};
    image_data.texture = destination;
    image_data.data_size = data_size;
    image_data.host_offset = aligned_host_offset;
    textures.push_back(image_data);

    host_data_used = aligned_host_offset + data_size;
}

void StagingBuffer::stageBuffer(const Buffer& destination, const void* data,
                                size_t data_size) {
    memcpy(buffer.getHostAddress() + host_data_used, data, data_size);

    BufferData buffer_data = {.buffer = destination,
                              .host_offset = host_data_used,
                              .data_size = data_size};
    buffers.push_back(buffer_data);

    host_data_used += data_size;
}

size_t StagingBuffer::getTexelBlockSize(VkFormat format) {
    if (format == VK_FORMAT_R8G8B8A8_SRGB) return 4;

    return 1;
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
        image_barriers.push_back(image_data.texture->createBarrier(
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

    auto buffer_handle = buffer.getState().buffer;

    for (auto& buffer_data : buffers) {
        VkBufferCopy copy = {};
        copy.srcOffset = buffer_data.host_offset;
        copy.size = buffer_data.data_size;

        vkCmdCopyBuffer(cmd.buffer, buffer_handle,
                        buffer_data.buffer.getState().buffer, 1, &copy);
    }

    for (auto& image_data : textures) {
        VkBufferImageCopy copy = {};
        copy.bufferOffset = image_data.host_offset;
        copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copy.imageSubresource.layerCount = 1;
        copy.imageExtent.width = image_data.texture->getWidth();
        copy.imageExtent.height = image_data.texture->getHeight();
        copy.imageExtent.depth = 1;

        vkCmdCopyBufferToImage(cmd.buffer, buffer_handle,
                               image_data.texture->getImage(),
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
    }

    textures.clear();
    buffers.clear();
    host_data_used = 0;
}

Buffer StagingBuffer::createBuffer(Device& device) {
    return BufferBuilder(BUFFER_SIZE)
        .isCopySource()
        .isDeviceAddressable()
        .isCPUWritable(true, true)
        .create(device)
        .getResult();
}

}  // namespace Graphics