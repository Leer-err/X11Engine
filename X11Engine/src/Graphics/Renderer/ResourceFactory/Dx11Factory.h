#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <dxgiformat.h>
#include <wrl/client.h>

#include <cstddef>
#include <memory>

#include "Format.h"
#include "GraphicsConfig.h"
#include "IPixelShader.h"
#include "IRenderContext.h"
#include "IResourceFactory.h"
#include "IShaderBlob.h"
#include "ISwapChain.h"
#include "ITexture.h"
#include "IVertexShader.h"
#include "Image.h"
#include "Logger.h"

class Dx11Factory : public IResourceFactory {
   public:
    Dx11Factory();

    std::shared_ptr<IRenderContext> getContext() const override;
    std::shared_ptr<ISwapChain> getSwapChain() const override;
    std::shared_ptr<ISwapChain> createSwapChain(uint32_t width, uint32_t height,
                                                bool is_windowed);

    ID3D11Device* getDeviceHandle() const;
    ID3D11DeviceContext* getContextHandle() const;

    std::shared_ptr<ITexture> createTexture(const Image& image,
                                            bool cpu_witable,
                                            bool gpu_writable) override;
    std::shared_ptr<ITexture> createTexture(ImageFormat format, uint32_t width,
                                            uint32_t height, bool cpu_witable,
                                            bool gpu_writable) override;

    std::shared_ptr<IRenderTarget> createRenderTarget() override;
    std::shared_ptr<IRenderTarget> createRenderTarget(
        std::shared_ptr<ITexture>);
    std::shared_ptr<IDepthStencil> createDepthStencil(uint32_t width,
                                                      uint32_t height) override;

    std::shared_ptr<IRasterizer> createRasterizerState() override;

    std::shared_ptr<IPixelShader> createPixelShader(
        IShaderBlob* shader_source) override;
    std::shared_ptr<IVertexShader> createVertexShader(
        IShaderBlob* shader_source) override;

    std::shared_ptr<IConstantBuffer> createConstantBuffer(
        const char* data, size_t size, bool cpu_writable) override;
    std::shared_ptr<IVertexBuffer> createVertexBuffer(
        const char* data, size_t size, bool cpu_writable) override;
    std::shared_ptr<IIndexBuffer> createIndexBuffer(const char* data,
                                                    size_t size,
                                                    bool cpu_writable) override;
    std::shared_ptr<IStructuredBuffer> createStructuredBuffer(
        const char* data, size_t size, size_t struct_size,
        bool cpu_writable) override;

   private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> createDxTexture(
        DXGI_FORMAT format, D3D11_BIND_FLAG bind, uint32_t width,
        uint32_t height);

    Microsoft::WRL::ComPtr<ID3D11InputLayout> createInputLayout(
        IShaderBlob* shader);

    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain;

    Microsoft::WRL::ComPtr<IDXGIFactory4> factory;

    Logger logger;
};