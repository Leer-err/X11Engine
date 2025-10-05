#pragma once

#include <dxgi.h>
#include <wrl/client.h>

#include "ISwapChain.h"

class Dx11SwapChain : public ISwapChain {
   public:
    Dx11SwapChain(Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain);

    void present();

   private:
    Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain;
};