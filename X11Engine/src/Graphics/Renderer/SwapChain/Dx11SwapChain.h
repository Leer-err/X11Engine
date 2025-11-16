#pragma once

#include <dxgi1_4.h>
#include <wrl/client.h>

#include "ISwapChain.h"

class Dx11SwapChain : public ISwapChain {
   public:
    Dx11SwapChain(Microsoft::WRL::ComPtr<IDXGISwapChain1> swap_chain);

    void present();

   private:
    Microsoft::WRL::ComPtr<IDXGISwapChain1> swap_chain;
};