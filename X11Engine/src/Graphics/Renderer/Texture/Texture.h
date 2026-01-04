#pragma once

#include <dxgiformat.h>
#include <wrl/client.h>

#include <cstdint>

#include "d3d11_1.h"

class Texture {
    friend class TextureBuilder;
    friend class SwapChain;
    friend class RenderTarget;
    friend class ShaderResource;
    friend class Context;

   public:
    Texture();

    uint32_t getWidth() const;
    uint32_t getHeight() const;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> get() const;

   protected:
    Texture(Microsoft::WRL::ComPtr<ID3D11Texture2D> texture, uint32_t width,
            uint32_t height, DXGI_FORMAT format);

    DXGI_FORMAT getFormat() const;

   private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;

    uint32_t width;
    uint32_t height;

    DXGI_FORMAT format;
};