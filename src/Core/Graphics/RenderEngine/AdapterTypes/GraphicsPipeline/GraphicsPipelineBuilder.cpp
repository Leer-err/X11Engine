#include "GraphicsPipelineBuilder.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <array>

#include "EngineData.h"
#include "GraphicsPipeline.h"
#include "InputLayout.h"
#include "InputLayoutBuilder.h"
#include "Rasterizer.h"
#include "Shader.h"
#include "ShaderBuilder.h"

namespace Graphics {

GraphicsPipelineBuilder::GraphicsPipelineBuilder(
    const std::string& vertex_shader_filename,
    const std::string& vertex_shader_entrypoint,
    const std::string& pixel_shader_filename,
    const std::string& pixel_shader_entrypoint)
    : vertex_shader_filename(vertex_shader_filename),
      vertex_shader_entrypoint(vertex_shader_entrypoint),
      pixel_shader_filename(pixel_shader_filename),
      pixel_shader_entrypoint(pixel_shader_entrypoint),
      render_target_format(VK_FORMAT_R8G8B8A8_SRGB),
      rasterization_state(Graphics::Rasterizer::fill()) {}

static VkPipelineShaderStageCreateInfo getStageInfo(const Shader& shader) {
    VkPipelineShaderStageCreateInfo stage_info = {};
    stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stage_info.stage = shader.stage;
    stage_info.module = shader.shader;
    stage_info.pName = shader.entrypoint.c_str();

    return stage_info;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::setRenderTargetFormat(
    VkFormat format) {
    render_target_format = format;
    return *this;
}

Result<GraphicsPipeline, GraphicsPipelineBuilder::Error>
GraphicsPipelineBuilder::create(const EngineData& engine_data) {
    auto vertex_shader_result =
        createShader(engine_data, vertex_shader_filename,
                     vertex_shader_entrypoint, VK_SHADER_STAGE_VERTEX_BIT);
    auto pixel_shader_result =
        createShader(engine_data, pixel_shader_filename,
                     pixel_shader_entrypoint, VK_SHADER_STAGE_FRAGMENT_BIT);

    if (vertex_shader_result.isError() || pixel_shader_result.isError())
        return Error::ShaderNotBuilt;

    auto vertex_shader = vertex_shader_result.getResult();
    auto pixel_shader = pixel_shader_result.getResult();

    auto input_layout_result =
        InputLayoutBuilder(engine_data.shader_registry, vertex_shader_filename)
            .create();

    if (input_layout_result.isError()) {
        switch (input_layout_result.getError()) {
            case InputLayoutBuilder::Error::FileNotFound:
            case InputLayoutBuilder::Error::ParseError:
                return Error::ShaderNotBuilt;
            case InputLayoutBuilder::Error::UnsupportedElementFormat:
                return Error::VertexInputTypeNotSupported;
        }
    }

    auto input_layout = input_layout_result.getResult();

    auto pipeline = GraphicsPipeline{};
    pipeline.layout = engine_data.device.createPipelineLayout(
        input_layout.push_constants_size);

    std::array shader_stages = {getStageInfo(vertex_shader),
                                getStageInfo(pixel_shader)};
    VkPipelineInputAssemblyStateCreateInfo input_assembly_state = {};
    input_assembly_state.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineVertexInputStateCreateInfo vertex_input_state = {};
    vertex_input_state.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_state.vertexBindingDescriptionCount = 1;
    vertex_input_state.pVertexBindingDescriptions =
        &input_layout.buffer_binding_description;
    vertex_input_state.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(input_layout.elements.size());
    vertex_input_state.pVertexAttributeDescriptions =
        input_layout.elements.data();

    VkPipelineViewportStateCreateInfo viewport_state = {};
    viewport_state.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    std::array dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT,
                                 VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamic_state = {};
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.dynamicStateCount = dynamic_states.size();
    dynamic_state.pDynamicStates = dynamic_states.data();

    VkPipelineDepthStencilStateCreateInfo depth_stencil_state = {};
    depth_stencil_state.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil_state.depthTestEnable = VK_TRUE;
    depth_stencil_state.depthWriteEnable = VK_TRUE;
    depth_stencil_state.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

    VkPipelineRenderingCreateInfo rendering = {};
    rendering.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    rendering.depthAttachmentFormat = VK_FORMAT_D24_UNORM_S8_UINT;
    rendering.stencilAttachmentFormat = VK_FORMAT_D24_UNORM_S8_UINT;
    rendering.colorAttachmentCount = 1;
    rendering.pColorAttachmentFormats = &render_target_format;

    VkPipelineColorBlendAttachmentState blendAttachment = {};
    blendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    blendAttachment.blendEnable = true;
    blendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    blendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    blendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    blendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo color_blend_state = {};
    color_blend_state.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_state.logicOpEnable = VK_FALSE;
    color_blend_state.logicOp = VK_LOGIC_OP_COPY;
    color_blend_state.attachmentCount = 1;
    color_blend_state.pAttachments = &blendAttachment;

    VkPipelineMultisampleStateCreateInfo multisampling_state = {};
    multisampling_state.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkGraphicsPipelineCreateInfo pipeline_info = {};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.pNext = &rendering;
    pipeline_info.stageCount = shader_stages.size();
    pipeline_info.pStages = shader_stages.data();
    pipeline_info.pVertexInputState = &vertex_input_state;
    pipeline_info.pInputAssemblyState = &input_assembly_state;
    pipeline_info.pViewportState = &viewport_state;
    pipeline_info.pMultisampleState = &multisampling_state;
    pipeline_info.pDepthStencilState = &depth_stencil_state;
    pipeline_info.pRasterizationState = &rasterization_state;
    pipeline_info.pColorBlendState = &color_blend_state;
    pipeline_info.pDynamicState = &dynamic_state;
    pipeline_info.layout = pipeline.layout;
    pipeline_info.flags = VK_PIPELINE_CREATE_DESCRIPTOR_BUFFER_BIT_EXT;

    return engine_data.device.createGraphicsPipeline(pipeline_info);
}

Result<Shader, GraphicsPipelineBuilder::Error>
GraphicsPipelineBuilder::createShader(const EngineData& engine_data,
                                      const std::string& filename,
                                      const std::string& entrypoint,
                                      VkShaderStageFlagBits stage) {
    auto shader_build_result =
        ShaderBuilder(engine_data, filename, entrypoint, stage).create();

    if (shader_build_result.isError()) return Error::ShaderNotBuilt;

    return shader_build_result.getResult();
}
}  // namespace Graphics