#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <optional>
#include <string>

#include "EngineData.h"
#include "GraphicsPipeline.h"

// #include "InputLayout.h"
#include "InputLayout.h"
#include "Result.h"
#include "Shader.h"
#include "ShaderRegistry.h"
// #include "RenderTarget.h"

namespace Graphics {

class GraphicsPipelineBuilder {
   public:
    enum class Error { ShaderNotBuilt, VertexInputTypeNotSupported };

    GraphicsPipelineBuilder(const std::string& vertex_shader_filename,
                            const std::string& vertex_shader_entrypoint,
                            const std::string& pixel_shader_filename,
                            const std::string& pixel_shader_entrypoint);

    GraphicsPipelineBuilder& setRasterizer(
        VkPipelineRasterizationStateCreateInfo rasterizer);

    GraphicsPipelineBuilder& setRenderTargetFormat(VkFormat format);

    Result<GraphicsPipeline, Error> create(Device& device,
                                           ShaderRegistry& shader_registry);

   private:
    Result<Shader, GraphicsPipelineBuilder::Error> createShader(
        Device& device, ShaderRegistry& shader_registry,
        const std::string& filename, const std::string& entrypoint,
        VkShaderStageFlagBits stage);

    const std::string& vertex_shader_filename;
    const std::string& vertex_shader_entrypoint;
    const std::string& pixel_shader_filename;
    const std::string& pixel_shader_entrypoint;

    VkFormat render_target_format;
    VkPipelineRasterizationStateCreateInfo rasterization_state;
};

}  // namespace Graphics