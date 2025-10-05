#ifndef DX11_RASTERIZER_H
#define DX11_RASTERIZER_H

#include <d3d11.h>
#include <wrl/client.h>

#include "IRasterizer.h"

class Dx11Rasterizer : public IRasterizer {
   public:
    Dx11Rasterizer(Microsoft::WRL::ComPtr<ID3D11RasterizerState> state);

    ID3D11RasterizerState* get();

   private:
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> state;
};

#endif  // DX11_RASTERIZER_H