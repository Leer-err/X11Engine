#pragma once

#include "DepthStencil.h"
#include "GraphicsConfig.h"
#include "RenderTarget.h"
#include "RenderTarget/RenderTarget.h"
#include "SwapChain.h"

class Renderer {
   public:
    static Renderer& get() {
        static Renderer instance;
        return instance;
    }

    void initializeResources(const GraphicsConfig& config);

    void beginFrame();
    void endFrame();

    RenderTarget getDefaultRenderTarget() const {
        return default_render_target;
    }
    DepthStencil getDefaultDepthStencilBuffer() const {
        return default_depth_stencil_buffer;
    }

   private:
    Renderer();

    Texture render_target_texture;
    RenderTarget default_render_target;
    DepthStencil default_depth_stencil_buffer;

    SwapChain swap_chain;
};