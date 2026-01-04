#pragma once

#include <dxgi1_4.h>
#include <wrl/client.h>

class Texture;

class SwapChain {
    friend class SwapChainBuilder;

   public:
    SwapChain() {}

    void present();

    Texture getBackbuffer();

   protected:
    SwapChain(const Microsoft::WRL::ComPtr<IDXGISwapChain1>& swap_chain);

   private:
    Microsoft::WRL::ComPtr<IDXGISwapChain1> swap_chain;
};