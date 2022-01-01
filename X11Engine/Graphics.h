#pragma once
#include <d3d11_4.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <memory>
#include "Helper.h"
#include <vector>

using Microsoft::WRL::ComPtr;
using std::unique_ptr;
using std::vector;
class SwapChain;
class Buffer;

class Graphics
{
public:
	Graphics(UINT width, UINT height, HWND hWnd);

	~Graphics();

	void PreFrame();
	void PostFrame();
	void Draw(const vector<float>& vertices, const vector<uint32_t>& indices);
private:
	ComPtr<IDXGIFactory7> m_factory;
	ComPtr<ID3D11Device5> m_device;
	ComPtr<ID3D11DeviceContext4> m_context;
	ComPtr<ID3D11RenderTargetView1> m_rtv;
	ComPtr<IDXGIAdapter4> m_adapter;
	ComPtr<ID3D11InputLayout> m_inputLayout;
	ComPtr<ID3D11PixelShader> m_pixelShader;
	ComPtr<ID3D11VertexShader> m_vertexShader;

	unique_ptr<SwapChain> m_swapChain;

	D3D_FEATURE_LEVEL m_featureLevel;
};

