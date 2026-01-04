#include "Texture.h"

#include "d3d11_1.h"

Texture::Texture() : texture(nullptr) {}

Texture::Texture(Microsoft::WRL::ComPtr<ID3D11Texture2D> texture,
                 uint32_t width, uint32_t height, DXGI_FORMAT format)
    : texture(texture), width(width), height(height), format(format) {}

Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture::get() const { return texture; }

uint32_t Texture::getWidth() const { return width; }

uint32_t Texture::getHeight() const { return height; }

DXGI_FORMAT Texture::getFormat() const { return format; }