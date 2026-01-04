#pragma once

#include <d3d11.h>
#include <wrl/client.h>

class InputLayout {
    friend class InputLayoutBuilder;
    friend class Context;

   public:
    InputLayout() = default;

   protected:
    InputLayout(Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout);

    Microsoft::WRL::ComPtr<ID3D11InputLayout> get() const;

   private:
    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
};
