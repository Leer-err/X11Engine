#pragma once

#include <vulkan/vulkan.h>

#include <optional>
#include <vector>

#include "GraphicsPipeline.h"
#include "Texture.h"

namespace Graphics {

struct PassDependency {
    Texture texture;
    VkImageLayout expected_layout;
};

class GraphicsPassDependencies {
   public:
    void reads(const Texture& texture);

    void writesColor(const Texture& texture);
    void writesDepth(const Texture& texture);

   private:
    std::vector<PassDependency> read_textures;
    std::vector<PassDependency> color_outputs;

    std::optional<PassDependency> depth_output;
};

class GraphicsPass {
   public:
    GraphicsPass(const GraphicsPipeline& pipeline);

    void execute();

   private:
    GraphicsPipeline pipeline;
};

class FrameGraph {
   public:
    struct Pass;

    void addGraphicsPass();
};

}  // namespace Graphics