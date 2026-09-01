#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <bit>
#include <cstddef>
#include <functional>
#include <optional>
#include <vector>

#include "CommandBuffer.h"
#include "DescriptorSet.h"
#include "EngineData.h"
#include "FrameData.h"
#include "GraphicsMesh.h"
#include "GraphicsPipeline.h"
#include "Texture.h"

namespace Graphics {

class GraphicsPassExecution {
   public:
    GraphicsPassExecution(const DescriptorSet& descriptor_set,
                          const CommandBuffer& command_buffer,
                          const GraphicsPipeline& pipeline);
    ~GraphicsPassExecution();

    template <typename T>
    void appendData(const T& data) {
        appendData(std::bit_cast<uint8_t*>(&data), sizeof(T));
    }
    void appendData(const uint8_t* data, size_t data_size);
    void draw(const Mesh& mesh);
    void draw(const Mesh& mesh, size_t count);

   private:
    CommandBuffer command_buffer;

    GraphicsPipeline pipeline;
    size_t current_push_constant_offset;
};

class Pass {
   public:
    struct PassTexture {
        VkImageLayout layout;
        Texture texture;
    };

    void reads(const Texture& texture, VkImageLayout layout);
    void writes(const Texture& texture, VkImageLayout layout);

    void prepareTextures(const CommandBuffer& command_buffer);

   private:
    static VkImageMemoryBarrier2 barrierFromLayout(PassTexture& texture);
    static VkImageMemoryBarrier2 fullBarrier(Texture& texture,
                                             VkImageLayout layout);
    static VkImageMemoryBarrier2 depthStencilBarrier(Texture& texture);
    static VkImageMemoryBarrier2 colorAttachmentBarrier(Texture& texture);
    static VkImageMemoryBarrier2 shaderResourceBarrier(Texture& texture);
    static VkImageMemoryBarrier2 copySourceBarrier(Texture& texture);
    static VkImageMemoryBarrier2 copyDestinatonBarrier(Texture& texture);

    std::vector<PassTexture> pass_textures;
};

class GraphicsPass : public Pass {
    struct Attachment {
        Texture texture;
        VkClearValue clear_value;
        bool clear;
    };

   public:
    GraphicsPass(const GraphicsPipeline& pipeline,
                 const std::function<void(GraphicsPassExecution&)>& executor);

    void addColorAttachment(const Texture& texture);
    void addColorAttachment(const Texture& texture, VkClearValue clear_color);

    void setDepthAttachment(const Texture& texture);
    void setDepthAttachment(const Texture& texture, VkClearValue clear_color);

    void execute(const DescriptorSet& descriptor_set,
                 const CommandBuffer& command_buffer);

   private:
    GraphicsPipeline pipeline;

    std::vector<Attachment> color_attachments;
    std::optional<Attachment> depth_attachment;

    std::function<void(GraphicsPassExecution&)> executor;
};

class GeneralPass : public Pass {
   public:
    GeneralPass(std::function<void(const CommandBuffer&)> executor);

    void execute(const CommandBuffer& command_buffer);

   private:
    std::function<void(const CommandBuffer&)> executor;
};

class FrameGraph {
   public:
    explicit FrameGraph(const EngineData& engine_data);

    void addGraphicsPass(const GraphicsPass& pass);

    void execute(const FrameData& frame_data);

   private:
    EngineData engine_data;

    std::vector<GraphicsPass> passes;
};

}  // namespace Graphics