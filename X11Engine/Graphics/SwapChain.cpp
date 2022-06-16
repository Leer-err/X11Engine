#include "SwapChain.h"
#include <d3d11_4.h>
#include <dxgi1_6.h>
#include "Logger/Logger.h"
#include "Window.h"
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

	if (FAILED(factory->CreateSwapChainForHwnd(device, hWnd, &desc, nullptr, nullptr, &swapChain))) {
		Logger::get().Error(L"Swap chain creation failed");
		Window::get().Terminate();
	}

	if (SUCCEEDED(swapChain.As(&m_swapChain))) {
		Logger::get().Debug(L"Swap chain created successfully");
	}
	else {
		Logger::get().Error(L"Swap chain creation failed");
		Window::get().Terminate();
	}
}

SwapChain::~SwapChain()
{
	m_swapChain->Release();
}

ID3D11Texture2D1* SwapChain::GetBuffer(UINT index)
{
	ID3D11Texture2D1* buffer;
	if (FAILED(m_swapChain->GetBuffer(index, IID_PPV_ARGS(&buffer)))) {
		Logger::get().Error(L"GetBuffer call failed");
		Window::get().Terminate();
	}
	return buffer;
}

void SwapChain::Present()
{
	if (FAILED(m_swapChain->Present(0, 0))) {
		Logger::get().Error(L"Present call failed");
		Window::get().Terminate();
	}
}
