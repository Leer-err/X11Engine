#pragma once

#include <d3d11.h>
#include <wrl/client.h>

class DepthStencil {
    friend class DepthStencilBuilder;
    friend class Context;

   public:
    DepthStencil() = default;

   protected:
    DepthStencil(Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil);

    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> get() const;

   private:
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil;
};