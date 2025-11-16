#pragma once

#include <d3d11.h>
#include <wrl/client.h>

class ShaderResource {
    friend class ShaderResourceBuilder;

   public:
   protected:
    ShaderResource(
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resoure);

   private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resoure;
};