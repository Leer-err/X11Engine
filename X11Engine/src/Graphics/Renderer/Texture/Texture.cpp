#include "Texture.h"

#include "d3d11_1.h"

Texture::Texture(Microsoft::WRL::ComPtr<ID3D11Texture2D> texture)
    : texture(texture) {}