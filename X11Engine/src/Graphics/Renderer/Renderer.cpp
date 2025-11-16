#include "Renderer.h"

#include <memory>
#include <tracy/Tracy.hpp>

#include "ResourceFactory.h"

Renderer::Renderer() {
    // auto factory = ResourceFactory::get();

    // context = factory->getContext();

    // rasterizer = factory->createRasterizerState();
    // context->setRasterizerState(rasterizer);
}

void Renderer::beginFrame() {
    ZoneScoped;
    // context->clearRenderTarget(render_target);
    // context->clearDepthStencil(depth_stencil);

    // context->setRenderTarget(render_target, depth_stencil);
}

void Renderer::endFrame() {
    ZoneScoped;
    // swap_chain->present();
}

void Renderer::initializeResources(const GraphicsConfig& config) {
    // auto factory = ResourceFactory::get();
    // swap_chain = factory->createSwapChain(
    //     config.render_width, config.render_height,
    //     config.window_state != WindowState::Fullscreen);

    // render_target = factory->createRenderTarget();
    // depth_stencil = factory->createDepthStencil(width, height);

    // context->setViewport(width, height);
}
