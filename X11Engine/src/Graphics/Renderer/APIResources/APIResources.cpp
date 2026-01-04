#include "APIResources.h"

#include <d3d11.h>
#include <d3d11_1.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <winnt.h>
#include <wrl/client.h>

#include "LoggerFactory.h"

using Microsoft::WRL::ComPtr;

APIResources::APIResources()
    : logger(LoggerFactory::getLogger("APIResources")) {
    D3D_FEATURE_LEVEL feature_levels[] = {D3D_FEATURE_LEVEL_11_0};
    UINT num_feature_levels = ARRAYSIZE(feature_levels);

    UINT creation_flags = 0;
#if defined(_DEBUG)
    creation_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL feature_level;
    ComPtr<ID3D11Device> temp_device;
    HRESULT result = D3D11CreateDevice(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, creation_flags, feature_levels,
        num_feature_levels, D3D11_SDK_VERSION, temp_device.GetAddressOf(),
        &feature_level, nullptr);

    result = temp_device.As(&device);
    if (SUCCEEDED(result)) {
        logger.debug("Created D3D device");
    } else {
        logger.error("Failed to create D3D device");
    }

    ComPtr<IDXGIDevice> dxgiDevice;
    result = device.As(&dxgiDevice);

    ComPtr<IDXGIAdapter> dxgiAdapter;
    result = dxgiDevice->GetAdapter(&dxgiAdapter);
    result = dxgiAdapter.As(&adapter);

    ComPtr<IDXGIFactory> dxgifactory;
    result = adapter->GetParent(IID_PPV_ARGS(&factory));

    device->GetImmediateContext1(&context);
}

ComPtr<ID3D11Device1> APIResources::getDevice() const { return device; }

ComPtr<IDXGIFactory4> APIResources::getFactory() const { return factory; }

ComPtr<IDXGIAdapter1> APIResources::getAdapter() const { return adapter; }

Microsoft::WRL::ComPtr<ID3D11DeviceContext1> APIResources::getContext() const {
    return context;
}
