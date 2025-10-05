#pragma once

#include <memory>

#include "IConstantBuffer.h"
#include "IDepthStencil.h"
#include "IIndexBuffer.h"
#include "IPixelShader.h"
#include "IRasterizer.h"
#include "IRenderContext.h"
#include "IRenderTarget.h"
#include "IShaderBlob.h"
#include "IStructuredBuffer.h"
#include "ISwapChain.h"
#include "ITexture.h"
#include "IVertexBuffer.h"
#include "IVertexShader.h"
#include "Image.h"

class IResourceFactory {
   public:
    virtual std::shared_ptr<IRenderContext> getContext() const = 0;
    virtual std::shared_ptr<ISwapChain> getSwapChain() const = 0;

    virtual std::shared_ptr<ITexture> createTexture(const Image& image,
                                                    bool cpu_witable,
                                                    bool gpu_writable) = 0;
    virtual std::shared_ptr<ITexture> createTexture(ImageFormat format,
                                                    uint32_t width,
                                                    uint32_t height,
                                                    bool cpu_witable,
                                                    bool gpu_writable) = 0;

    virtual std::shared_ptr<IRenderTarget> createRenderTarget() = 0;
    virtual std::shared_ptr<IDepthStencil> createDepthStencil(
        uint32_t width, uint32_t height) = 0;

    virtual std::shared_ptr<IRasterizer> createRasterizerState() = 0;

    virtual std::shared_ptr<IPixelShader> createPixelShader(
        IShaderBlob* shader_source) = 0;
    virtual std::shared_ptr<IVertexShader> createVertexShader(
        IShaderBlob* shader_source) = 0;

    virtual std::shared_ptr<IConstantBuffer> createConstantBuffer(
        const char* data, size_t size, bool cpu_writable) = 0;
    virtual std::shared_ptr<IVertexBuffer> createVertexBuffer(
        const char* data, size_t size, bool cpu_writable) = 0;
    virtual std::shared_ptr<IIndexBuffer> createIndexBuffer(
        const char* data, size_t size, bool cpu_writable) = 0;
    virtual std::shared_ptr<IStructuredBuffer> createStructuredBuffer(
        const char* data, size_t count, size_t struct_size,
        bool cpu_writable) = 0;
};