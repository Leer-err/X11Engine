#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "Format.h"
#include "ITexture.h"

class Dx11Texture : public ITexture {
   public:
    Dx11Texture(Microsoft::WRL::ComPtr<ID3D11Texture2D> texture,
                Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view,
                Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler,
                uint32_t width, uint32_t height);

    ImageFormat getFormat() const;

    ID3D11ShaderResourceView* getView();
    ID3D11SamplerState* getSampler();

   private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
    uint32_t width;
    uint32_t height;
};