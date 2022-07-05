#pragma once

#include <d3d11.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class Texture {
public:
	Texture() = default;
	Texture(int width, int height, const void* pData);

	ID3D11Texture2D* GetTexture() const { return m_texture.Get(); }
private:
	ComPtr<ID3D11Texture2D> m_texture;
};