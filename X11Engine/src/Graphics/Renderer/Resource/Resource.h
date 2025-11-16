#pragma once

#include <d3d11.h>
#include <dxgiformat.h>
#include <wrl/client.h>

#include <concepts>

template <typename T>
concept Resource = requires(T resource) {
    {
        resource.get()
    } -> std::convertible_to<Microsoft::WRL::ComPtr<ID3D11Resource>>;
    { resource.getFormat() } -> std::convertible_to<DXGI_FORMAT>;
};