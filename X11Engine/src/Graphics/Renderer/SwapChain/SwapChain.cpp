#include "SwapChain.h"

#include "dxgi1_4.h"

SwapChain::SwapChain(const Microsoft::WRL::ComPtr<IDXGISwapChain1>& swap_chain)
    : swap_chain(swap_chain) {}

void SwapChain::present() { swap_chain->Present1(0, 0, nullptr); }
