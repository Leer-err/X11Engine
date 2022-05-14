#include "SwapChain.h"
#include <d3d11_4.h>
#include <dxgi1_6.h>
#include "../Helper.h"

SwapChain::SwapChain(UINT width, UINT height, HWND hWnd, IDXGIFactory7* factory, ID3D11Device5* device)
{
	ComPtr<IDXGISwapChain1> swapChain;
	DXGI_SWAP_CHAIN_DESC1 desc = {};
	desc.BufferCount = 2;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Width = width;
	desc.Height = height;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	ThrowIfFailed(factory->CreateSwapChainForHwnd(device, hWnd, &desc, nullptr, nullptr, &swapChain));
	ThrowIfFailed(swapChain.As(&m_swapChain));
}

SwapChain::~SwapChain()
{
	m_swapChain->Release();
}

ID3D11Texture2D1* SwapChain::GetBuffer(UINT index)
{
	ID3D11Texture2D1* buffer;
	ThrowIfFailed(m_swapChain->GetBuffer(index, IID_PPV_ARGS(&buffer)));
	return buffer;
}

void SwapChain::Present()
{
	ThrowIfFailed(m_swapChain->Present(0, 0));
}
