#include "InputLayout.h"

InputLayout::InputLayout(Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout)
    : input_layout(input_layout) {}

Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayout::get() const {
    return input_layout.Get();
}