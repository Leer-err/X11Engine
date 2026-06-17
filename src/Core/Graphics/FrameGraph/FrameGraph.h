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

   private:
    CommandBuffer command_buffer;

    GraphicsPipeline pipeline;
    size_t current_push_constant_offset;
};

class GraphicsPass {
    struct Attachment {
        Texture texture;
        bool clear;
        VkClearValue clear_value;
    };

   public:
    GraphicsPass(
        const GraphicsPipeline& pipeline,
        const std::function<void(GraphicsPassExecution&)>& pass_function);

    void reads(const Texture& texture);

    void addColorAttachment(const Texture& texture, bool clear,
                            VkClearValue clear_color);
    void addDepthAttachment(const Texture& texture, bool writes, bool clear,
                            VkClearValue clear_value);

    void execute(const DescriptorSet& descriptor_set,
                 const FrameData& frame_data);

   private:
    GraphicsPipeline pipeline;
    std::function<void(GraphicsPassExecution&)> pass_function;

    std::vector<Texture> read_textures;
    std::vector<Attachment> color_attachments;

    std::optional<Attachment> depth_attachment;
    bool writes_depth;
};

class Pass {
    struct PassTexture {
        VkImageLayout layout;
        Texture texture;
    };

   public:
    void reads(const Texture& texture, VkImageLayout layout);
    void writes(const Texture& texture, VkImageLayout layout);

    void setGeneralExecutor();
    void setGraphicsExecutor(
        std::function<void(const GraphicsPassExecution&)> executor);

   private:
    std::vector<PassTexture> pass_textures;
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