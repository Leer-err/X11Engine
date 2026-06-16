#include "BufferBuilder.h"

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include <array>

#include "Buffer.h"
#include "Device.h"
#include "EngineConstants.h"
#include "Result.h"

namespace Graphics {

BufferBuilder::BufferBuilder(size_t size) : buffer_info(), alloc_info() {
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;

    alloc_info.usage = VMA_MEMORY_USAGE_AUTO;
}

BufferBuilder& BufferBuilder::isShaderResource() {
    buffer_info.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                         VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    return *this;
}

BufferBuilder& BufferBuilder::isVertexBuffer() {
    buffer_info.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    return *this;
}

BufferBuilder& BufferBuilder::isIndexBuffer() {
    buffer_info.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    return *this;
}

BufferBuilder& BufferBuilder::isConstantBuffer() {
    buffer_info.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT |
                         VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    return *this;
}

BufferBuilder& BufferBuilder::isDescriptorBuffer() {
    buffer_info.usage |= VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT |
                         VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    return *this;
}

BufferBuilder& BufferBuilder::isCPUWritable(bool always_mapped,
                                            bool is_random) {
    if (is_random)
        alloc_info.flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
    else
        alloc_info.flags |=
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    if (always_mapped) alloc_info.flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;

    return *this;
}

BufferBuilder& BufferBuilder::isDeviceAddressable() {
    buffer_info.usage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    return *this;
}

BufferBuilder& BufferBuilder::isCopySource() {
    buffer_info.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    return *this;
}

BufferBuilder& BufferBuilder::isCopyDestination() {
    buffer_info.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    return *this;
}

BufferBuilder& BufferBuilder::isChained() {
    is_chained = true;
    return *this;
}

Result<Buffer, BufferError> BufferBuilder::create(
    Device& device, BufferRegistry& registry) const {
    if (is_chained)
        createChained(device, registry);
    else
        createSingle(device, registry);
}

Result<Buffer, BufferError> BufferBuilder::createSingle(
    Device& device, BufferRegistry& registry) const {
    auto state_opt = device.createBuffer(buffer_info, alloc_info);
    if (state_opt.isError()) return state_opt.getError();

    registry.registerBuffer(state_opt.getResult());
}

Result<Buffer, BufferError> BufferBuilder::createChained(
    Device& device, BufferRegistry& registry) const {
    std::array<BufferState, MAX_FRAMES_IN_FLIGHT> buffer_states;

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        auto state_opt = device.createBuffer(buffer_info, alloc_info);
        if (state_opt.isError()) return state_opt.getError();

        buffer_states[i] = state_opt.getResult();
    }

    registry.registerBufferChain(buffer_states);
}

}  // namespace Graphics