#include "Texture.h"

#include <dxgi1_6.h>

#include "Graphics/Graphics.h"

Texture::Texture(int width, int height, const void* pData) {
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.ArraySize = 1;
	desc.MipLevels = 1;
	desc.Width = width;
	desc.Height = height;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA res = {};
	res.pSysMem = pData;
	res.SysMemPitch = width * 4;
	res.SysMemSlicePitch = width * height * 4;

	Graphics::get().GetDevice()->CreateTexture2D(&desc, &res, m_texture.GetAddressOf());
}