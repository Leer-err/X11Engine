#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "Texture.h"

class RenderTarget {
    friend class RenderTargetBuilder;
    friend class Context;

   public:
    RenderTarget() = default;
    RenderTarget(const Texture& texture);

    static RenderTarget getDefault();

   protected:
    RenderTarget(Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target);

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> get() const;
    D3D11_VIEWPORT getViewport() const;

   private:
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target;
    D3D11_VIEWPORT viewport;
};