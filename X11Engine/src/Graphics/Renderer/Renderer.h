#pragma once

#include <memory>

#include "ICamera.h"
#include "IConstantBuffer.h"
#include "IDepthStencil.h"
#include "IRasterizer.h"
#include "IRenderContext.h"
#include "IRenderTarget.h"
#include "IResourceFactory.h"
#include "IStructuredBuffer.h"
#include "ISwapChain.h"

class Renderer {
   public:
    Renderer();

    void init(std::shared_ptr<IResourceFactory> factory, uint32_t width,
              uint32_t height);

    void beginFrame();
    void endFrame();

   private:
    std::shared_ptr<ISwapChain> swap_chain;
    std::shared_ptr<IRenderContext> context;
    std::shared_ptr<IResourceFactory> factory;

    std::shared_ptr<IRenderTarget> render_target;
    std::shared_ptr<IDepthStencil> depth_stencil;

    std::shared_ptr<IRasterizer> rasterizer;

    std::shared_ptr<IConstantBuffer> camera_buffer;
    std::shared_ptr<IStructuredBuffer> bone_buffer;
};