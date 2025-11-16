#include "SwapChainBuilder.h"

#include <d3d11_1.h>
#include <dxgi1_4.h>

#include "APIResources.h"
#include "Window.h"

SwapChainBuilder::SwapChainBuilder(uint32_t width, uint32_t height)
    : width(width), height(height), is_fullscreen(false) {}

SwapChainBuilder& SwapChainBuilder::windowed() {
    is_fullscreen = false;
    return *this;
}

SwapChainBuilder& SwapChainBuilder::fullscren() {
    is_fullscreen = true;
    return *this;
}

SwapChain SwapChainBuilder::create() {
    DXGI_SWAP_CHAIN_DESC1 desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.Stereo = FALSE;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 2;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreen_desc = {};
    fullscreen_desc.RefreshRate.Numerator = 0;
    fullscreen_desc.RefreshRate.Denominator = 1;
    fullscreen_desc.Windowed = !is_fullscreen;

    auto factory = APIResources::get().getFactory();
    auto device = APIResources::get().getDevice();

    auto window = Window::get().getHandle();

    Microsoft::WRL::ComPtr<IDXGISwapChain1> swap_chain;
    HRESULT result = factory->CreateSwapChainForHwnd(
        device.Get(), window, &desc, &fullscreen_desc, NULL, &swap_chain);

    return SwapChain(swap_chain);
}