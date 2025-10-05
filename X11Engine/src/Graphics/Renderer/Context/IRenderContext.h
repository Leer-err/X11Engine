#pragma once

#include <memory>

#include "IConstantBuffer.h"
#include "IDepthStencil.h"
#include "IIndexBuffer.h"
#include "IPixelShader.h"
#include "IRasterizer.h"
#include "IRenderTarget.h"
#include "IStructuredBuffer.h"
#include "ITexture.h"
#include "IVertexBuffer.h"
#include "IVertexShader.h"
#include "ShaderType.h"

class IRenderContext {
   public:
    virtual void setViewport(uint32_t width, uint32_t height) = 0;

    virtual void clearRenderTarget(
        const std::shared_ptr<IRenderTarget> render_target) = 0;
    virtual void clearDepthStencil(
        const std::shared_ptr<IDepthStencil> depth_stencil) = 0;

    virtual void setRasterizerState(
        const std::shared_ptr<IRasterizer> rasterizer_state) = 0;

    virtual void setVertexBuffer(
        const std::shared_ptr<IVertexBuffer> vertex_buffer) = 0;
    virtual void setIndexBuffer(
        const std::shared_ptr<IIndexBuffer> index_buffer) = 0;

    virtual void setVertexShader(
        const std::shared_ptr<IVertexShader> vertex_shader) = 0;
    virtual void setPixelShader(
        const std::shared_ptr<IPixelShader> pixel_shader) = 0;

    virtual void draw(uint32_t index_count) = 0;

    virtual void bindConstantBuffer(ShaderType type,
                                    std::shared_ptr<IConstantBuffer> buffer,
                                    uint32_t reg) = 0;
    virtual void bindStructuredBuffer(ShaderType type,
                                      std::shared_ptr<IStructuredBuffer> buffer,
                                      uint32_t reg) = 0;

    virtual void bindTexture(ShaderType type, std::shared_ptr<ITexture> texture,
                             uint32_t reg) = 0;

    virtual void setRenderTarget(
        std::shared_ptr<IRenderTarget> target,
        std::shared_ptr<IDepthStencil> depth_stencil) = 0;
};