#include "GraphicsPipelineBuilder.h"

#include "CommonRasterizers.h"
#include "GraphicsPipeline.h"
#include "InputLayout.h"
#include "PixelShader.h"
#include "VertexShader.h"

GraphicsPipelineBuilder::GraphicsPipelineBuilder(InputLayout input_layout,
                                                 VertexShader vertex_shader,
                                                 PixelShader pixel_shader)
    : input_layout(input_layout),
      vertex_shader(vertex_shader),
      pixel_shader(pixel_shader),
      rasterizer(Engine::Graphics::CommonRasterizers::fill()),
      default_render_target(true),
      has_depth_stencil(false) {}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::setRenderTarget(
    RenderTarget render_target) {
    this->render_target = render_target;
    default_render_target = false;

    return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::setDepthStencilBuffer(
    DepthStencil depth_stencil_buffer) {
    this->depth_stencil_buffer = depth_stencil_buffer;

    return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::setRasterizerState(
    Engine::Graphics::Rasterizer rasterizer) {
    this->rasterizer = rasterizer;

    return *this;
}

GraphicsPipeline GraphicsPipelineBuilder::create() {
    if (default_render_target) render_target = RenderTarget::getDefault();

    return GraphicsPipeline(input_layout, vertex_shader, pixel_shader,
                            render_target, depth_stencil_buffer, rasterizer);
}
