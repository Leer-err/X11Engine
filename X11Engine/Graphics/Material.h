#pragma once
#include <wrl/client.h>
#include <d3d11_4.h>

using Microsoft::WRL::ComPtr;

struct Material {
	ComPtr<ID3D11PixelShader> pixelShader;
	ComPtr<ID3D11VertexShader> vertexShader;

	ComPtr<ID3D11ShaderResourceView> baseColor;
	ComPtr<ID3D11ShaderResourceView> diffuse;
	ComPtr<ID3D11ShaderResourceView> specular;
	ComPtr<ID3D11ShaderResourceView> emission;
};