#pragma once

#include <wrl/client.h>

class IDXGISwapChain1;

class SwapChain {
    friend class SwapChainBuilder;

   public:
    void present();

    // Texture getBackbuffer();

   protected:
    SwapChain(const Microsoft::WRL::ComPtr<IDXGISwapChain1>& swap_chain);

   private:
    Microsoft::WRL::ComPtr<IDXGISwapChain1> swap_chain;
};