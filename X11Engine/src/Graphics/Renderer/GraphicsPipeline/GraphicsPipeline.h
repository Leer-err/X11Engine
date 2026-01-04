#pragma once

#include "DepthStencil.h"
#include "InputLayout.h"
#include "PixelShader.h"
#include "RenderTarget.h"
#include "VertexShader.h"

class GraphicsPipeline {
    friend class GraphicsPipelineBuilder;

   public:
    GraphicsPipeline() = default;

    InputLayout getInputLayout() const { return input_layout; }

    VertexShader getVertexShader() const { return vertex_shader; }
    PixelShader getPixelShader() const { return pixel_shader; }
    RenderTarget getRenderTarget() const { return render_target; }
    DepthStencil getDepthStencil() const { return depth_stencil; }

   protected:
    GraphicsPipeline(InputLayout input_layout, VertexShader vertex_shader,
                     PixelShader pixel_shader, RenderTarget render_target,
                     DepthStencil depth_stencil)
        : vertex_shader(vertex_shader),
          pixel_shader(pixel_shader),
          input_layout(input_layout),
          render_target(render_target),
          depth_stencil(depth_stencil) {}

   private:
    VertexShader vertex_shader;
    PixelShader pixel_shader;

    InputLayout input_layout;

    RenderTarget render_target;
    DepthStencil depth_stencil;
};