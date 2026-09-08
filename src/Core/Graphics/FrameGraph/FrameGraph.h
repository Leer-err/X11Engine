#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <bit>
#include <cstddef>
#include <functional>
#include <optional>
#include <string_view>
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
        TextureHandle texture;
    };

    explicit Pass(std::string_view name);

    void reads(TextureHandle texture, VkImageLayout layout);
    void writes(TextureHandle texture, VkImageLayout layout);

    void prepareTextures(const CommandBuffer& command_buffer);

    std::string_view getName() const;

   private:
    static VkImageMemoryBarrier2 barrierFromLayout(PassTexture& texture);
    static VkImageMemoryBarrier2 fullBarrier(TextureHandle texture,
                                             VkImageLayout layout);
    static VkImageMemoryBarrier2 depthStencilBarrier(TextureHandle texture);
    static VkImageMemoryBarrier2 colorAttachmentBarrier(TextureHandle texture);
    static VkImageMemoryBarrier2 shaderResourceBarrier(TextureHandle texture);
    static VkImageMemoryBarrier2 copySourceBarrier(TextureHandle texture);
    static VkImageMemoryBarrier2 copyDestinatonBarrier(TextureHandle texture);

    std::vector<PassTexture> pass_textures;
    std::string name;
};

class GraphicsPass : public Pass {
    struct Attachment {
        TextureHandle texture;
        VkClearValue clear_value;
        bool clear;
    };

   public:
    GraphicsPass(std::string_view name, const GraphicsPipeline& pipeline,
                 const std::function<void(GraphicsPassExecution&)>& executor);

    void addColorAttachment(TextureHandle texture);
    void addColorAttachment(TextureHandle texture, VkClearValue clear_color);

    void setDepthAttachment(TextureHandle texture);
    void setDepthAttachment(TextureHandle texture, VkClearValue clear_color);

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
    GeneralPass(std::string_view name,
                std::function<void(const CommandBuffer&)> executor);

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