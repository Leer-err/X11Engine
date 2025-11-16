#pragma once

#include <wrl/client.h>

#include "d3d11_1.h"

class Texture {
    friend class TextureBuilder;

   public:
   protected:
    Texture(Microsoft::WRL::ComPtr<ID3D11Texture2D> texture);

   private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
};