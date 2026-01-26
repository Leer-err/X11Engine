#pragma once

#include <d3d11_1.h>
#include <wrl/client.h>

namespace Engine::Graphics {

class Rasterizer {
    friend class Context;
    friend class RasterizerBuilder;

   public:
    Rasterizer() = default;

   protected:
    Rasterizer(Microsoft::WRL::ComPtr<ID3D11RasterizerState1> rasterizer)
        : rasterizer(rasterizer) {}

    Microsoft::WRL::ComPtr<ID3D11RasterizerState1> get() const;

   private:
    Microsoft::WRL::ComPtr<ID3D11RasterizerState1> rasterizer;
};

};  // namespace Engine::Graphics
