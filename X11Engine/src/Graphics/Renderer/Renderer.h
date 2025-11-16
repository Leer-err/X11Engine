#pragma once

#include <memory>

#include "GraphicsConfig.h"
#include "IDepthStencil.h"
#include "IRasterizer.h"
#include "IRenderContext.h"
#include "IRenderTarget.h"
#include "ISwapChain.h"

class Renderer {
   public:
    Renderer();

    void beginFrame();
    void endFrame();

   private:
    void initializeResources(const GraphicsConfig& config);

    std::shared_ptr<ISwapChain> swap_chain;
    std::shared_ptr<IRenderContext> context;

    std::shared_ptr<IRenderTarget> render_target;
    std::shared_ptr<IDepthStencil> depth_stencil;

    std::shared_ptr<IRasterizer> rasterizer;
};