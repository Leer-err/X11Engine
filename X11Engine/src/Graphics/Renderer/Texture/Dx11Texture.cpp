#include "Dx11Texture.h"

#include <d3d11.h>

#include "Format.h"

Dx11Texture::Dx11Texture(Microsoft::WRL::ComPtr<ID3D11Texture2D> texture,
                         Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view,
                         Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler,
                         uint32_t width, uint32_t height)
    : texture(texture),
      view(view),
      sampler(sampler),
      width(width),
      height(height) {}

ImageFormat Dx11Texture::getFormat() const { return ImageFormat::BGR_24BPP; }

ID3D11ShaderResourceView* Dx11Texture::getView() { return view.Get(); }

ID3D11SamplerState* Dx11Texture::getSampler() { return sampler.Get(); }
