#include "Renderer.h"

#include <d3d11.h>
#include <d3d11_1.h>
#include <wrl/client.h>

#include <tracy/Tracy.hpp>

#include "APIResources.h"
#include "Context.h"
#include "Format.h"
#include "SwapChainBuilder.h"
#include "TextureBuilder.h"

Renderer::Renderer() {
    // auto factory = ResourceFactory::get();

    // context = factory->getContext();

    // rasterizer = factory->createRasterizerState();
    // context->setRasterizerState(rasterizer);
}

void Renderer::beginFrame() {
    ZoneScoped;
    auto context = Context();

    context.clean(default_render_target);

    // context->clearRenderTarget(render_target);
    // context->clearDepthStencil(depth_stencil);

    // context->setRenderTarget(render_target, depth_stencil);
}

void Renderer::endFrame() {
    ZoneScoped;
    auto context = Context();

    auto backbuffer = swap_chain.getBackbuffer();
    context.copy(render_target_texture, backbuffer);

    swap_chain.present();
}

void Renderer::initializeResources(const GraphicsConfig& config) {
    width = config.render_width;
    height = config.render_height;

    swap_chain = SwapChainBuilder(config.render_width, config.render_height)
                     .windowed()
                     .create();

    render_target_texture =
        TextureBuilder(ImageFormat::RGBA_32BPP, config.render_width,
                       config.render_height)
            .isGPUWritable()
            .isRenderTarget()
            .create()
            .getResult();

    default_render_target = RenderTarget(render_target_texture);

    auto context = APIResources::get().getContext();
    auto device = APIResources::get().getDevice();

    D3D11_BLEND_DESC1 desc = {};
    desc.RenderTarget[0].BlendEnable = TRUE;
    desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    Microsoft::WRL::ComPtr<ID3D11BlendState1> blend_state;
    device->CreateBlendState1(&desc, &blend_state);

    context->OMSetBlendState(blend_state.Get(), nullptr, 0xffffffff);

    // auto factory = ResourceFactory::get();
    // swap_chain = factory->createSwapChain(
    //     config.render_width, config.render_height,
    //     config.window_state != WindowState::Fullscreen);

    // render_target = factory->createRenderTarget();
    // depth_stencil = factory->createDepthStencil(width, height);

    // context->setViewport(width, height);
}

uint32_t Renderer::getWidth() const { return width; }

uint32_t Renderer::getHeight() const { return height; }
