#pragma once

#include <d3d11.h>
#include <wrl/client.h>

class RenderTarget {
    friend class RenderTargetBuilder;

   public:
   protected:
    RenderTarget(Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target);

   private:
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target;
};