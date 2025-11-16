#include "ResourceFactory.h"

#include <d3d11_1.h>
#include <d3dcommon.h>
#include <dxgi1_4.h>
#include <winerror.h>
#include <winnt.h>

#include "LoggerFactory.h"
#include "SwapChain.h"
#include "Window.h"

ResourceFactory::ResourceFactory()
    : logger(LoggerFactory::getLogger("ResourceFactory")) {
    createDXGIFactory();
    createDXDevice();
}

void ResourceFactory::createDXGIFactory() {
    Microsoft::WRL::ComPtr<IDXGIFactory> factory1;
    HRESULT result = CreateDXGIFactory1(IID_PPV_ARGS(&factory1));

    result = factory1.As(&factory);
    if (SUCCEEDED(result)) {
        logger.debug("Got DXGI factory");
    } else {
        logger.error("Failed to create DXGI factory");
    }
}

void ResourceFactory::createDXDevice() {
    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
    HRESULT result = factory->EnumAdapters1(0, adapter.GetAddressOf());
    logger.debug("Got adapter");

    D3D_FEATURE_LEVEL feature_levels[] = {D3D_FEATURE_LEVEL_11_0};
    UINT num_feature_levels = ARRAYSIZE(feature_levels);

    UINT creation_flags = 0;
#if defined(_DEBUG)
    creation_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL feature_level;
    result = D3D11CreateDevice(
        adapter.Get(), D3D_DRIVER_TYPE_HARDWARE, NULL, creation_flags,
        feature_levels, num_feature_levels, D3D11_SDK_VERSION,
        device.GetAddressOf(), &feature_level, context.GetAddressOf());

    logger.debug("Created device");
}

// SwapChain ResourceFactory::createSwapChain(uint32_t width, uint32_t height,
//                                            bool is_fullscreen) {
//     DXGI_SWAP_CHAIN_DESC1 desc = {};
//     desc.Width = width;
//     desc.Height = height;
//     desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//     desc.Stereo = FALSE;
//     desc.SampleDesc.Count = 1;
//     desc.SampleDesc.Quality = 0;
//     desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//     desc.BufferCount = 2;
//     desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

//     DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreen_desc = {};
//     fullscreen_desc.RefreshRate.Numerator = 0;
//     fullscreen_desc.RefreshRate.Denominator = 1;
//     fullscreen_desc.Windowed = !is_fullscreen;

//     Microsoft::WRL::ComPtr<IDXGISwapChain1> swap_chain;
//     HRESULT result = factory->CreateSwapChainForHwnd(
//         device.Get(), Window::get().getHandle(), &desc, &fullscreen_desc,
//         NULL, &swap_chain);

//     return SwapChain(swap_chain);
// }

// RenderContext ResourceFactory::createContext() {
//     return RenderContext(context);
// }

// Result<Texture, ResourceError> ResourceFactory::createTexture(
//     const Image& image, bool cpu_witable, bool gpu_writable) {
//     ImageFormat format = image.getFormat();
//     uint32_t width = image.getWidth();
//     auto height = image.getHeight();
//     auto data = image.getData();

//     auto dxgi_format_optional = imageFormatToDXGIFormat(format);
//     if (dxgi_format_optional == std::nullopt)
//         return ResourceError::UnsupportedFormat;
//     auto dxgi_format = dxgi_format_optional.value();

//     D3D11_TEXTURE2D_DESC desc = {};
//     desc.Format = dxgi_format;
//     desc.ArraySize = 1;
//     desc.MipLevels = 1;
//     desc.Width = width;
//     desc.Height = height;
//     desc.SampleDesc.Count = 1;
//     desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

//     if (!cpu_witable && !gpu_writable) {
//         desc.Usage = D3D11_USAGE_IMMUTABLE;
//     } else if (cpu_witable && !gpu_writable) {
//         desc.Usage = D3D11_USAGE_DYNAMIC;
//         desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//     } else if (!cpu_witable && gpu_writable) {
//         desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
//         desc.Usage = D3D11_USAGE_DEFAULT;
//     } else {
//         return ResourceError::WriteFromGPUAndCPU;
//     }

//     size_t bytes_per_pixel = imageFormatBytesPerPixel(format);

//     D3D11_SUBRESOURCE_DATA subRes = {};
//     subRes.pSysMem = data;
//     subRes.SysMemPitch = width * bytes_per_pixel;
//     subRes.SysMemSlicePitch = width * height * bytes_per_pixel;

//     Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
//     auto result = device->CreateTexture2D(&desc, &subRes, &texture);
//     if (FAILED(result)) return nullptr;

//     D3D11_SHADER_RESOURCE_VIEW_DESC view_descriptor = {};
//     // view_descriptor.Format = dxgi_format;
//     view_descriptor.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
//     view_descriptor.Texture2D.MipLevels = 1;

//     Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view;
//     result = device->CreateShaderResourceView(texture.Get(),
//     &view_descriptor,
//                                               &view);
//     if (FAILED(result)) return nullptr;

//     D3D11_SAMPLER_DESC sampler_descriptor = {};
//     sampler_descriptor.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
//     sampler_descriptor.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
//     sampler_descriptor.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
//     sampler_descriptor.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
//     sampler_descriptor.MipLODBias = 0;
//     sampler_descriptor.ComparisonFunc = D3D11_COMPARISON_ALWAYS;

//     Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
//     result = device->CreateSamplerState(&sampler_descriptor, &sampler);
//     if (FAILED(result)) return nullptr;

//     return std::make_shared<Dx11Texture>(texture, view, sampler, width,
//     height);
// }