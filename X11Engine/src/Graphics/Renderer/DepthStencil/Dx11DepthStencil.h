#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "IDepthStencil.h"

class Dx11DepthStencil : public IDepthStencil {
   public:
    Dx11DepthStencil(Microsoft::WRL::ComPtr<ID3D11DepthStencilView> view,
                     Microsoft::WRL::ComPtr<ID3D11DepthStencilState> state);

    ID3D11DepthStencilView* get();
    ID3D11DepthStencilState* getState();

   private:
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> state;
};