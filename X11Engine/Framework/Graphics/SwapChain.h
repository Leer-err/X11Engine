#pragma once
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

struct IDXGIFactory7;
struct IDXGISwapChain4;
struct ID3D11Device5;
struct ID3D11Texture2D1;

class SwapChain {
   public:
    SwapChain(UINT width, UINT height, HWND hWnd, IDXGIFactory7* factory,
              ID3D11Device5* device);
    ~SwapChain();

    ID3D11Texture2D1* GetBuffer(UINT index);

    void Present();

   private:
    ComPtr<IDXGISwapChain4> m_swapChain;
};
