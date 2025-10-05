#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "IRenderTarget.h"

class Dx11RenderTarget : public IRenderTarget {
   public:
    Dx11RenderTarget(
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target);

    ID3D11RenderTargetView* get();

   private:
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target;
};