#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "IConstantBuffer.h"
#include "IDepthStencil.h"
#include "IRenderContext.h"
#include "IStructuredBuffer.h"
#include "ITexture.h"
#include "Logger.h"
#include "ShaderType.h"

class Dx11Context : public IRenderContext {
   public:
    Dx11Context(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

    void setViewport(uint32_t width, uint32_t height) override;

    void clearRenderTarget(
        const std::shared_ptr<IRenderTarget> render_target) override;
    void clearDepthStencil(
        const std::shared_ptr<IDepthStencil> depth_stencil) override;

    void setVertexBuffer(
        const std::shared_ptr<IVertexBuffer> vertex_buffer) override;
    void setIndexBuffer(
        const std::shared_ptr<IIndexBuffer> index_buffer) override;

    void setVertexShader(
        const std::shared_ptr<IVertexShader> vertex_shader) override;
    void setPixelShader(
        const std::shared_ptr<IPixelShader> pixel_shader) override;

    void draw(uint32_t index_count) override;

    void bindConstantBuffer(ShaderType type,
                            std::shared_ptr<IConstantBuffer> buffer,
                            uint32_t reg) override;
    void bindStructuredBuffer(ShaderType type,
                              std::shared_ptr<IStructuredBuffer> buffer,
                              uint32_t reg) override;

    void bindTexture(ShaderType type, std::shared_ptr<ITexture> texture,
                     uint32_t reg) override;

    void setRenderTarget(std::shared_ptr<IRenderTarget> target,
                         std::shared_ptr<IDepthStencil> depth_stencil) override;
    void setRasterizerState(
        const std::shared_ptr<IRasterizer> rasterizer_state) override;

   private:
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

    Logger logger;
};