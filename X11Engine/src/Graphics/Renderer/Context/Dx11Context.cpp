#include "Dx11Context.h"

#include <d3d11.h>
#include <dxgiformat.h>
#include <minwindef.h>
#include <winerror.h>

#include <tracy/Tracy.hpp>

#include "Dx11ConstantBuffer.h"
#include "Dx11DepthStencil.h"
#include "Dx11IndexBuffer.h"
#include "Dx11PixelShader.h"
#include "Dx11Rasterizer.h"
#include "Dx11RenderTarget.h"
#include "Dx11StructuredBuffer.h"
#include "Dx11Texture.h"
#include "Dx11VertexBuffer.h"
#include "Dx11VertexShader.h"
#include "LoggerFactory.h"
#include "ShaderType.h"
#include "Vertex.h"


Dx11Context::Dx11Context(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
    : context(context), logger(LoggerFactory::getLogger("Dx11Context")) {
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Dx11Context::setViewport(uint32_t width, uint32_t height) {
    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<FLOAT>(width);
    viewport.Height = static_cast<FLOAT>(height);
    viewport.MaxDepth = 1.0f;

    context->RSSetViewports(1, &viewport);
}

void Dx11Context::clearRenderTarget(
    const std::shared_ptr<IRenderTarget> render_target) {
    constexpr float CLEAR_COLOR[4] = {0.f, 0.f, 0.f, 0.f};

    auto target = reinterpret_cast<Dx11RenderTarget*>(render_target.get());
    context->ClearRenderTargetView(target->get(), CLEAR_COLOR);
}

void Dx11Context::clearDepthStencil(
    const std::shared_ptr<IDepthStencil> depth_stencil) {
    constexpr float CLEAR_DEPTH = 1.f;
    constexpr uint8_t CLEAR_STENCIL = 0.f;

    auto dx_depth_stencil =
        reinterpret_cast<Dx11DepthStencil*>(depth_stencil.get());
    context->ClearDepthStencilView(dx_depth_stencil->get(),
                                   D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
                                   CLEAR_DEPTH, CLEAR_STENCIL);
}

void Dx11Context::setVertexBuffer(
    const std::shared_ptr<IVertexBuffer> vertex_buffer) {
    auto dx_vertex_buffer = static_cast<Dx11VertexBuffer*>(vertex_buffer.get());

    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    ID3D11Buffer* buffer_ptr = dx_vertex_buffer->get();

    context->IASetVertexBuffers(0, 1, &buffer_ptr, &stride, &offset);
}

void Dx11Context::setIndexBuffer(
    const std::shared_ptr<IIndexBuffer> index_buffer) {
    auto dx_index_buffer = static_cast<Dx11IndexBuffer*>(index_buffer.get());

    UINT stride = 0;
    UINT offset = 0;

    ID3D11Buffer* buffer_ptr = dx_index_buffer->get();

    context->IASetIndexBuffer(buffer_ptr, DXGI_FORMAT_R32_UINT, 0);
}

void Dx11Context::setVertexShader(
    const std::shared_ptr<IVertexShader> vertex_shader) {
    auto dx_vertex_shader = static_cast<Dx11VertexShader*>(vertex_shader.get());

    context->VSSetShader(dx_vertex_shader->get(), nullptr, 0);
    context->IASetInputLayout(dx_vertex_shader->getInputLayout());
}

void Dx11Context::setPixelShader(
    const std::shared_ptr<IPixelShader> pixel_shader) {
    auto dx_pixel_shader = static_cast<Dx11PixelShader*>(pixel_shader.get());

    context->PSSetShader(dx_pixel_shader->get(), nullptr, 0);
}

void Dx11Context::draw(uint32_t index_count) {
    ZoneScoped;
    context->DrawIndexed(index_count, 0, 0);
}

void Dx11Context::bindConstantBuffer(ShaderType type,
                                     std::shared_ptr<IConstantBuffer> buffer,
                                     uint32_t reg) {
    Dx11ConstantBuffer* dx_buffer =
        static_cast<Dx11ConstantBuffer*>(buffer.get());

    ID3D11Buffer* buffer_handle = dx_buffer->get();
    switch (type) {
        case ShaderType::Vertex:
            context->VSSetConstantBuffers(reg, 1, &buffer_handle);
            break;
        case ShaderType::Pixel:
            context->PSSetConstantBuffers(reg, 1, &buffer_handle);
            break;
    }
}

void Dx11Context::bindStructuredBuffer(
    ShaderType type, std::shared_ptr<IStructuredBuffer> buffer, uint32_t reg) {
    Dx11StructuredBuffer* dx_buffer =
        static_cast<Dx11StructuredBuffer*>(buffer.get());

    ID3D11ShaderResourceView* buffer_handle = dx_buffer->getView();
    switch (type) {
        case ShaderType::Vertex:
            context->VSSetShaderResources(reg, 1, &buffer_handle);
            break;
        case ShaderType::Pixel:
            context->PSSetShaderResources(reg, 1, &buffer_handle);
            break;
    }
}

void Dx11Context::bindTexture(ShaderType type,
                              std::shared_ptr<ITexture> texture, uint32_t reg) {
    Dx11Texture* dx_texture = static_cast<Dx11Texture*>(texture.get());

    ID3D11ShaderResourceView* texture_view = dx_texture->getView();
    ID3D11SamplerState* sampler = dx_texture->getSampler();

    switch (type) {
        case ShaderType::Vertex:
            context->PSSetSamplers(reg, 1, &sampler);
            context->VSSetShaderResources(reg, 1, &texture_view);
            break;
        case ShaderType::Pixel:
            context->PSSetSamplers(reg, 1, &sampler);
            context->PSSetShaderResources(reg, 1, &texture_view);
            break;
    }
}

void Dx11Context::setRenderTarget(
    std::shared_ptr<IRenderTarget> render_target,
    std::shared_ptr<IDepthStencil> depth_stencil) {
    auto target = reinterpret_cast<Dx11RenderTarget*>(render_target.get());
    auto rt = target->get();

    auto dx_depth_stencil =
        reinterpret_cast<Dx11DepthStencil*>(depth_stencil.get());
    auto ds = dx_depth_stencil->get();

    context->OMSetRenderTargets(1, &rt, ds);
    context->OMSetDepthStencilState(dx_depth_stencil->getState(), 1);
}

void Dx11Context::setRasterizerState(
    const std::shared_ptr<IRasterizer> rasterizer_state) {
    auto state = reinterpret_cast<Dx11Rasterizer*>(rasterizer_state.get());

    context->RSSetState(state->get());
}