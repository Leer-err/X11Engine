#pragma once

#include <sys/types.h>
#include <vulkan/vulkan.h>

#include <array>
#include <cstddef>

#include "DescriptorSet.h"
#include "GraphicsPipeline.h"
#include "RenderEnviroment.h"
#include "TextureState.h"

namespace Graphics {

struct CommandBuffer {
    void begin() const;
    void end() const;

    void draw(uint32_t meshlet_count) const;

    void copy(const TextureState& src, TextureState& dst) const;
    void blit(const TextureState& src, TextureState& dst) const;

    template <typename T>
    void pushConstants(const GraphicsPipeline& pipeline, const T* constants,
                       size_t constant_offset) const {
        pushConstants(pipeline, constants, constant_offset, sizeof(T));
    }
    void pushConstants(const GraphicsPipeline& pipeline, const void* constants,
                       size_t constant_offset, size_t constant_size) const;

    void bindDescriptorSet(const GraphicsPipeline& pipeline,
                           const DescriptorSet& set) const;
    void setPipeline(const GraphicsPipeline& pipeline) const;

    void bindRenderEnviroment(const RenderEnviroment& env) const;
    void unbindRenderEnviroment() const;

    void barrier(const VkImageMemoryBarrier2* image_barriers,
                 size_t image_barrier_count,
                 const VkBufferMemoryBarrier2* buffer_barriers,
                 size_t buffer_barrier_count) const;
    void barrier(
        const std::span<VkImageMemoryBarrier2>& image_barriers,
        const std::span<VkBufferMemoryBarrier2>& buffer_barriers) const {
        barrier(image_barriers.data(), image_barriers.size(),
                buffer_barriers.data(), buffer_barriers.size());
    }
    void barrier(
        const std::span<VkBufferMemoryBarrier2>& buffer_barriers) const {
        barrier(nullptr, 0, buffer_barriers.data(), buffer_barriers.size());
    }
    void barrier(const std::span<VkImageMemoryBarrier2>& image_barriers) const {
        barrier(image_barriers.data(), image_barriers.size(), nullptr, 0);
    }

    void reset() const;

    VkCommandBufferSubmitInfo submit() const;

    VkCommandBuffer buffer;
};

}  // namespace Graphics