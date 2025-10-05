#include "Renderer.h"

#include <memory>

#include "Matrix.h"
#include "Model.h"
#include "Shader/ShaderType.h"

Renderer::Renderer() {}

void Renderer::init(std::shared_ptr<IResourceFactory> factory, uint32_t width,
                    uint32_t height) {
    this->factory = factory;

    context = factory->getContext();
    swap_chain = factory->getSwapChain();

    render_target = factory->createRenderTarget();
    depth_stencil = factory->createDepthStencil(width, height);

    rasterizer = factory->createRasterizerState();
    context->setRasterizerState(rasterizer);

    camera_buffer =
        factory->createConstantBuffer(nullptr, sizeof(Matrix), true);
    bone_buffer = factory->createStructuredBuffer(nullptr, sizeof(Matrix) * 100,
                                                  sizeof(Matrix), true);

    context->setViewport(width, height);

    context->bindConstantBuffer(ShaderType::Vertex, camera_buffer, 0);
}

void Renderer::beginFrame() {
    context->clearRenderTarget(render_target);
    context->clearDepthStencil(depth_stencil);

    context->setRenderTarget(render_target, depth_stencil);
}

void Renderer::endFrame() { swap_chain->present(); }
