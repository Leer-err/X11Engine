#include "SwapChain.h"

#include <combaseapi.h>
#include <d3d11.h>

#include "Texture.h"
#include "dxgi1_4.h"

SwapChain::SwapChain(const Microsoft::WRL::ComPtr<IDXGISwapChain1>& swap_chain)
    : swap_chain(swap_chain) {}

void SwapChain::present() { swap_chain->Present(0, 0); }

Texture SwapChain::getBackbuffer() {
    Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer;
    HRESULT hr = swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
    if (FAILED(hr)) return Texture();

    D3D11_TEXTURE2D_DESC desc;
    back_buffer->GetDesc(&desc);

    return Texture(back_buffer, desc.Width, desc.Height, desc.Format);
}
