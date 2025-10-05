#include "Dx11SwapChain.h"

Dx11SwapChain::Dx11SwapChain(Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain)
    : swap_chain(swap_chain) {}

void Dx11SwapChain::present() { swap_chain->Present(0, 0); }