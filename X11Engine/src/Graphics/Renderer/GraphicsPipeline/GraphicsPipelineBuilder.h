#pragma once

#include "DepthStencil.h"
#include "GraphicsPipeline.h"
#include "InputLayout.h"
#include "PixelShader.h"
#include "RenderTarget.h"
#include "VertexShader.h"

class GraphicsPipelineBuilder {
   public:
    GraphicsPipelineBuilder(InputLayout input_layout,
                            VertexShader vertex_shader,
                            PixelShader pixel_shader);

    GraphicsPipelineBuilder& setRenderTarget(RenderTarget render_target);
    GraphicsPipelineBuilder& setDepthStencilBuffer(
        DepthStencil depth_stencil_buffer);

    GraphicsPipelineBuilder& setRasterizerState(
        Engine::Graphics::Rasterizer rasterizer);

    GraphicsPipeline create();

   private:
    bool default_render_target;
    bool has_depth_stencil;

    RenderTarget render_target;
    DepthStencil depth_stencil_buffer;

    InputLayout input_layout;

    VertexShader vertex_shader;
    PixelShader pixel_shader;

    Engine::Graphics::Rasterizer rasterizer;
};