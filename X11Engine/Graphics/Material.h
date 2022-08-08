#pragma once
#include <wrl/client.h>
#include <d3d11_4.h>

using Microsoft::WRL::ComPtr;

struct Material {
	ComPtr<ID3D11InputLayout> inputLayout;
	ComPtr<ID3D11PixelShader> pixelShader;
	ComPtr<ID3D11VertexShader> vertexShader;

	ComPtr<ID3D11Texture2D> baseColor;
	ComPtr<ID3D11Texture2D> diffuse;
	ComPtr<ID3D11Texture2D> specular;
	ComPtr<ID3D11Texture2D> emission;
};