#include "Context.h"

#include <d3d11.h>
#include <d3d11_1.h>
#include <d3dcommon.h>
#include <dxgi1_4.h>
#include <dxgiformat.h>

#include "APIResources.h"
#include "Texture.h"

Context::Context() { context = APIResources::get().getContext(); }

void Context::setPipeline(const GraphicsPipeline& pipeline) {
    ID3D11RenderTargetView* render_target =
        pipeline.getRenderTarget().get().Get();
    ID3D11DepthStencilView* depth_stencil =
        pipeline.getDepthStencil().get().Get();
    context->OMSetRenderTargets(1, &render_target, depth_stencil);

    auto viewport = pipeline.getRenderTarget().getViewport();
    context->RSSetViewports(1, &viewport);

    auto input_layout = pipeline.getInputLayout().get();
    context->IASetInputLayout(input_layout.Get());

    auto vertex_shader = pipeline.getVertexShader().get();
    auto pixel_shader = pipeline.getPixelShader().get();
    context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    context->PSSetShader(pixel_shader.Get(), nullptr, 0);
}

void Context::cleanPipeline() {
    context->OMSetRenderTargets(0, nullptr, nullptr);

    ID3D11Buffer*
        constant_buffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT] =
            {};
    context->VSSetConstantBuffers(
        0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT,
        &constant_buffers[0]);
    context->PSSetConstantBuffers(
        0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT,
        &constant_buffers[0]);

    ID3D11SamplerState* samplers[D3D11_COMMONSHADER_SAMPLER_REGISTER_COUNT] =
        {};
    context->VSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_REGISTER_COUNT,
                           &samplers[0]);
    context->PSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_REGISTER_COUNT,
                           &samplers[0]);

    ID3D11ShaderResourceView*
        resources[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};
    context->VSSetShaderResources(
        0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, &resources[0]);
    context->PSSetShaderResources(
        0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, &resources[0]);
}

void Context::draw(const Mesh& mesh) {
    ID3D11Buffer* vertex_buffers[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {
        mesh.vertex_buffer.get().Get()};
    UINT offsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {
        static_cast<UINT>(mesh.vertex_buffer.getOffset())};
    UINT strides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {
        static_cast<UINT>(mesh.vertex_buffer.getStride())};

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetVertexBuffers(0, 1, vertex_buffers, strides, offsets);
    context->IASetIndexBuffer(mesh.index_buffer.get().Get(),
                              DXGI_FORMAT_R32_UINT,
                              mesh.index_buffer.getOffset());

    auto index_count =
        static_cast<UINT>(mesh.index_buffer.getSize() / sizeof(UINT));
    context->DrawIndexed(index_count, 0, 0);
}

void Context::copy(const Texture& source, Texture destination) {
    context->CopyResource(destination.get().Get(), source.get().Get());
}

void* Context::internalMap(Buffer buffer) {
    D3D11_MAPPED_SUBRESOURCE mapped_resource;

    context->Map(buffer.get().Get(), 0, D3D11_MAP_WRITE_DISCARD, NULL,
                 &mapped_resource);

    return mapped_resource.pData;
}

void Context::unmapConstantBuffer(const Buffer& buffer) {
    context->Unmap(buffer.get().Get(), 0);
}

void Context::bindConstantBuffer(const Buffer& buffer, const Binding& binding) {
    auto constant_buffer = buffer.get().Get();

    switch (binding.shader_type) {
        case Binding::ShaderType::Vertex:
            context->VSSetConstantBuffers(binding.index, 1, &constant_buffer);
            return;
        case Binding::ShaderType::Pixel:
            context->PSSetConstantBuffers(binding.index, 1, &constant_buffer);
            return;
    }
}

void Context::bindShaderResource(const ShaderResource& resource,
                                 const Binding& binding) {
    auto view = resource.get().Get();

    switch (binding.shader_type) {
        case Binding::ShaderType::Vertex:
            context->VSSetShaderResources(binding.index, 1, &view);
            return;
        case Binding::ShaderType::Pixel:
            context->PSSetShaderResources(binding.index, 1, &view);
            return;
    }
}

void Context::bindSampler(const Sampler& sampler, const Binding& binding) {
    auto sampler_state = sampler.get().Get();

    switch (binding.shader_type) {
        case Binding::ShaderType::Vertex:
            context->VSSetSamplers(binding.index, 1, &sampler_state);
            return;
        case Binding::ShaderType::Pixel:
            context->PSSetSamplers(binding.index, 1, &sampler_state);
            return;
    }
}

void Context::clean(RenderTarget render_target) {
    constexpr FLOAT clear_color[4] = {};

    context->ClearRenderTargetView(render_target.get().Get(), clear_color);
}
