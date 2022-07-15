#pragma once
#include <wrl/client.h>
#include <d3d11_4.h>

using Microsoft::WRL::ComPtr;

struct Material {
	ComPtr<ID3D11ShaderResourceView> texture;
};