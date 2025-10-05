#include "Dx11Factory.h"

#include <combaseapi.h>
#include <d3d11.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgiformat.h>
#include <dxgitype.h>
#include <intsafe.h>
#include <winerror.h>
#include <winnt.h>
#include <wrl/client.h>

#include <cstddef>
#include <memory>
#include <optional>

#include "Dx11ConstantBuffer.h"
#include "Dx11Context.h"
#include "Dx11DepthStencil.h"
#include "Dx11IndexBuffer.h"
#include "Dx11PixelShader.h"
#include "Dx11Rasterizer.h"
#include "Dx11RenderTarget.h"
#include "Dx11StructuredBuffer.h"
#include "Dx11SwapChain.h"
#include "Dx11Texture.h"
#include "Dx11VertexBuffer.h"
#include "Dx11VertexShader.h"
#include "Format.h"
#include "IShaderBlob.h"
#include "LoggerFactory.h"

constexpr std::optional<DXGI_FORMAT> imageFormatToDXGIFormat(
    ImageFormat format) {
    switch (format) {
        case ImageFormat::BGRA_32BPP:
            return DXGI_FORMAT_R8G8B8A8_UINT;
        case ImageFormat::RGBA_32BPP:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        default:
            return std::nullopt;
    }
}

Dx11Factory::Dx11Factory(HWND window, uint32_t width, uint32_t height)
    : logger(LoggerFactory::getLogger("Dx11Factory")) {
    Microsoft::WRL::ComPtr<IDXGIFactory> factory;
    HRESULT result = CreateDXGIFactory(IID_PPV_ARGS(&factory));
    logger.debug("Got factory");

    Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
    result = factory->EnumAdapters(0, adapter.GetAddressOf());
    logger.debug("Got adapter");

    D3D_FEATURE_LEVEL feature_levels[] = {D3D_FEATURE_LEVEL_11_0};
    UINT num_feature_levels = ARRAYSIZE(feature_levels);

    DXGI_SWAP_CHAIN_DESC desc = {};
    desc.BufferCount = 2;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferDesc.Width = width;
    desc.BufferDesc.Height = height;
    desc.BufferDesc.RefreshRate.Numerator = 0;
    desc.BufferDesc.RefreshRate.Denominator = 1;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.OutputWindow = window;
    desc.Windowed = true;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    UINT creation_flags = 0;
#if defined(_DEBUG)
    creation_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL feature_level;
    result = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, creation_flags, feature_levels,
        num_feature_levels, D3D11_SDK_VERSION, &desc, swap_chain.GetAddressOf(),
        device.GetAddressOf(), &feature_level, context.GetAddressOf());

    logger.debug("Created resources");
}

std::shared_ptr<IRenderContext> Dx11Factory::getContext() const {
    return std::make_shared<Dx11Context>(context);
}
std::shared_ptr<ISwapChain> Dx11Factory::getSwapChain() const {
    return std::make_shared<Dx11SwapChain>(swap_chain);
}

ID3D11Device* Dx11Factory::getDeviceHandle() const { return device.Get(); }

ID3D11DeviceContext* Dx11Factory::getContextHandle() const {
    return context.Get();
}

std::shared_ptr<ITexture> Dx11Factory::createTexture(const Image& image,
                                                     bool cpu_witable,
                                                     bool gpu_writable) {
    ImageFormat format = image.getFormat();
    uint32_t width = image.getWidth();
    auto height = image.getHeight();
    auto data = image.getData();

    auto dxgi_format_optional = imageFormatToDXGIFormat(format);
    if (dxgi_format_optional == std::nullopt) return nullptr;
    auto dxgi_format = dxgi_format_optional.value();

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Format = dxgi_format;
    desc.ArraySize = 1;
    desc.MipLevels = 1;
    desc.Width = width;
    desc.Height = height;
    desc.SampleDesc.Count = 1;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    if (!cpu_witable && !gpu_writable) {
        desc.Usage = D3D11_USAGE_IMMUTABLE;
    } else if (cpu_witable && !gpu_writable) {
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    } else if (!cpu_witable && gpu_writable) {
        desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
        desc.Usage = D3D11_USAGE_DEFAULT;
    } else {
        return nullptr;
    }

    size_t bytes_per_pixel = imageFormatBytesPerPixel(format);

    D3D11_SUBRESOURCE_DATA subRes = {};
    subRes.pSysMem = data;
    subRes.SysMemPitch = width * bytes_per_pixel;
    subRes.SysMemSlicePitch = width * height * bytes_per_pixel;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    auto result = device->CreateTexture2D(&desc, &subRes, &texture);
    if (FAILED(result)) return nullptr;

    D3D11_SHADER_RESOURCE_VIEW_DESC view_descriptor = {};
    // view_descriptor.Format = dxgi_format;
    view_descriptor.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    view_descriptor.Texture2D.MipLevels = 1;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view;
    result = device->CreateShaderResourceView(texture.Get(), &view_descriptor,
                                              &view);
    if (FAILED(result)) return nullptr;

    D3D11_SAMPLER_DESC sampler_descriptor = {};
    sampler_descriptor.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampler_descriptor.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampler_descriptor.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampler_descriptor.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampler_descriptor.MipLODBias = 0;
    sampler_descriptor.ComparisonFunc = D3D11_COMPARISON_ALWAYS;

    Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
    result = device->CreateSamplerState(&sampler_descriptor, &sampler);
    if (FAILED(result)) return nullptr;

    return std::make_shared<Dx11Texture>(texture, view, sampler, width, height);
}

std::shared_ptr<ITexture> Dx11Factory::createTexture(ImageFormat format,
                                                     uint32_t width,
                                                     uint32_t height,
                                                     bool cpu_witable,
                                                     bool gpu_writable) {
    auto dxgi_format_optional = imageFormatToDXGIFormat(format);
    if (dxgi_format_optional == std::nullopt) return nullptr;
    auto dxgi_format = dxgi_format_optional.value();

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Format = dxgi_format;
    desc.ArraySize = 1;
    desc.MipLevels = 1;
    desc.Width = width;
    desc.Height = height;
    desc.SampleDesc.Count = 1;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    if (!cpu_witable && !gpu_writable) {
        desc.Usage = D3D11_USAGE_IMMUTABLE;
    } else if (cpu_witable && !gpu_writable) {
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    } else if (!cpu_witable && gpu_writable) {
        desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
        desc.Usage = D3D11_USAGE_DEFAULT;
    } else {
        return nullptr;
    }

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    auto result = device->CreateTexture2D(&desc, nullptr, &texture);
    if (FAILED(result)) return nullptr;

    D3D11_SHADER_RESOURCE_VIEW_DESC view_descriptor = {};
    view_descriptor.Format = dxgi_format;
    view_descriptor.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    view_descriptor.Texture2D.MipLevels = 1;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view;
    result = device->CreateShaderResourceView(texture.Get(), &view_descriptor,
                                              &view);
    if (FAILED(result)) return nullptr;

    D3D11_SAMPLER_DESC sampler_descriptor = {};
    sampler_descriptor.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampler_descriptor.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    sampler_descriptor.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    sampler_descriptor.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    sampler_descriptor.MipLODBias = 0;
    sampler_descriptor.ComparisonFunc = D3D11_COMPARISON_NEVER;

    Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
    result = device->CreateSamplerState(&sampler_descriptor, &sampler);
    if (FAILED(result)) return nullptr;

    return std::make_shared<Dx11Texture>(texture, view, sampler, width, height);
}

std::shared_ptr<IRenderTarget> Dx11Factory::createRenderTarget() {
    D3D11_RENDER_TARGET_VIEW_DESC desc = {};
    desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.Texture2D.MipSlice = 0;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    swap_chain->GetBuffer(0, IID_PPV_ARGS(&texture));

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target;
    auto result = device->CreateRenderTargetView(texture.Get(), &desc,
                                                 render_target.GetAddressOf());

    if (FAILED(result)) return nullptr;

    return std::make_shared<Dx11RenderTarget>(render_target);
}

std::shared_ptr<IDepthStencil> Dx11Factory::createDepthStencil(
    uint32_t width, uint32_t height) {
    D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
    desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.Texture2D.MipSlice = 0;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture = createDxTexture(
        DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL, width, height);

    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil;
    auto result = device->CreateDepthStencilView(texture.Get(), &desc,
                                                 depth_stencil.GetAddressOf());
    if (FAILED(result)) return nullptr;

    D3D11_DEPTH_STENCIL_DESC state_desc = {};
    state_desc.DepthEnable = true;
    state_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    state_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    state_desc.StencilEnable = false;

    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> state;
    device->CreateDepthStencilState(&state_desc, state.GetAddressOf());

    return std::make_shared<Dx11DepthStencil>(depth_stencil, state);
}

std::shared_ptr<IRasterizer> Dx11Factory::createRasterizerState() {
    D3D11_RASTERIZER_DESC desc = {};
    desc.FillMode = D3D11_FILL_SOLID;
    desc.CullMode = D3D11_CULL_BACK;
    desc.FrontCounterClockwise = true;
    desc.DepthClipEnable = true;

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_state;
    HRESULT result =
        device->CreateRasterizerState(&desc, rasterizer_state.GetAddressOf());

    if (FAILED(result)) return nullptr;

    return std::make_shared<Dx11Rasterizer>(rasterizer_state);
}

std::shared_ptr<IPixelShader> Dx11Factory::createPixelShader(
    IShaderBlob* shader_source) {
    Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;
    auto result = device->CreatePixelShader(shader_source->get(),
                                            shader_source->getSize(), nullptr,
                                            shader.GetAddressOf());

    if (FAILED(result)) {
        logger.warning("Pixel shader was not created");
        return nullptr;
    }

    logger.debug("Craeted pixel shader");

    return std::make_shared<Dx11PixelShader>(shader);
}

std::shared_ptr<IVertexShader> Dx11Factory::createVertexShader(
    IShaderBlob* shader_source) {
    Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
    auto result = device->CreateVertexShader(shader_source->get(),
                                             shader_source->getSize(), nullptr,
                                             shader.GetAddressOf());

    if (FAILED(result)) {
        logger.warning("Vertex shader was not created");
        return nullptr;
    }

    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout =
        createInputLayout(shader_source);

    return std::make_shared<Dx11VertexShader>(shader, input_layout);
}

std::shared_ptr<IConstantBuffer> Dx11Factory::createConstantBuffer(
    const char* data, size_t size, bool cpu_writable) {
    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = size;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    if (cpu_writable) {
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.Usage = D3D11_USAGE_DYNAMIC;
    } else {
        desc.Usage = D3D11_USAGE_IMMUTABLE;
    }

    D3D11_SUBRESOURCE_DATA* initial_data = nullptr;

    D3D11_SUBRESOURCE_DATA res = {};
    res.pSysMem = data;
    if (data != nullptr) initial_data = &res;

    Microsoft::WRL::ComPtr<ID3D11Buffer> buf;
    auto result = device->CreateBuffer(&desc, initial_data, buf.GetAddressOf());
    if (FAILED(result)) {
        logger.warning("Failed to create constant buffer with {}", result);
        return nullptr;
    }

    return std::make_shared<Dx11ConstantBuffer>(buf, context);
}

std::shared_ptr<IVertexBuffer> Dx11Factory::createVertexBuffer(
    const char* data, size_t size, bool cpu_writable) {
    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = size;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    if (cpu_writable) {
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    } else {
        desc.Usage = D3D11_USAGE_IMMUTABLE;
    }

    D3D11_SUBRESOURCE_DATA subRes = {};
    subRes.pSysMem = data;

    D3D11_SUBRESOURCE_DATA* pInitialData = data == nullptr ? nullptr : &subRes;

    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
    HRESULT result =
        device->CreateBuffer(&desc, pInitialData, buffer.GetAddressOf());
    if (FAILED(result)) {
        logger.warning("Failed to create vertex buffer with {}", result);
        return nullptr;
    }

    return std::make_shared<Dx11VertexBuffer>(buffer, context);
}

std::shared_ptr<IIndexBuffer> Dx11Factory::createIndexBuffer(
    const char* data, size_t size, bool cpu_writable) {
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = size;
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    if (cpu_writable) {
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    } else {
        desc.Usage = D3D11_USAGE_IMMUTABLE;
    }

    D3D11_SUBRESOURCE_DATA subRes = {};
    subRes.pSysMem = data;

    D3D11_SUBRESOURCE_DATA* pInitialData = data == nullptr ? nullptr : &subRes;

    HRESULT result =
        device->CreateBuffer(&desc, pInitialData, buffer.GetAddressOf());
    if (FAILED(result)) {
        logger.warning("Failed to create index buffer with {}", result);
        return nullptr;
    }

    return std::make_shared<Dx11IndexBuffer>(buffer, size, context);
}
std::shared_ptr<IStructuredBuffer> Dx11Factory::createStructuredBuffer(
    const char* data, size_t count, size_t struct_size, bool cpu_writable) {
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = count * struct_size;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.StructureByteStride = struct_size;
    desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

    D3D11_SUBRESOURCE_DATA subRes = {};
    subRes.pSysMem = data;

    D3D11_SUBRESOURCE_DATA* pInitialData = data == nullptr ? nullptr : &subRes;

    if (!cpu_writable) {
        desc.Usage = D3D11_USAGE_IMMUTABLE;
    } else if (cpu_writable) {
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    }

    HRESULT result =
        device->CreateBuffer(&desc, pInitialData, buffer.GetAddressOf());
    if (FAILED(result)) {
        logger.warning("Failed to create structured buffer with {}", result);
        return nullptr;
    }

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view;
    result = device->CreateShaderResourceView(buffer.Get(), nullptr, &view);
    if (FAILED(result)) return nullptr;

    return std::make_shared<Dx11StructuredBuffer>(buffer, view, context);
}

Microsoft::WRL::ComPtr<ID3D11Texture2D> Dx11Factory::createDxTexture(
    DXGI_FORMAT format, D3D11_BIND_FLAG bind, uint32_t width, uint32_t height) {
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Format = format;
    desc.ArraySize = 1;
    desc.MipLevels = 1;
    desc.Width = width;
    desc.Height = height;
    desc.SampleDesc.Count = 1;
    desc.BindFlags = bind;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    auto result =
        device->CreateTexture2D(&desc, nullptr, texture.GetAddressOf());
    if (FAILED(result)) {
        logger.warning("Failed to create texture buffer with {}", result);
        return nullptr;
    }

    return texture;
}

Microsoft::WRL::ComPtr<ID3D11InputLayout> Dx11Factory::createInputLayout(
    IShaderBlob* shader) {
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> reflection;
    D3D11_SHADER_DESC shader_descriptor;
    std::vector<D3D11_INPUT_ELEMENT_DESC> input_elements;

    D3DReflect(shader->get(), shader->getSize(),
               IID_PPV_ARGS(reflection.GetAddressOf()));

    reflection->GetDesc(&shader_descriptor);

    for (UINT i = 0; i < shader_descriptor.InputParameters; i++) {
        D3D11_SIGNATURE_PARAMETER_DESC parameter;
        reflection->GetInputParameterDesc(i, &parameter);

        if (parameter.Mask > 0xf) {
            logger.error("Shader input element has more than 4 components");
        }

        if (parameter.ComponentType == D3D_REGISTER_COMPONENT_UNKNOWN) {
            logger.error("Shader input element has no type");
        }

        D3D11_INPUT_ELEMENT_DESC element;
        element.SemanticName = parameter.SemanticName;
        element.SemanticIndex = parameter.SemanticIndex;
        element.InputSlot = 0;
        element.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        element.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        element.InstanceDataStepRate = 0;

        uint32_t parameter_type =
            parameter.Mask | (parameter.ComponentType << 4);

        switch (parameter_type) {
            case 0x11:
                element.Format = DXGI_FORMAT_R32_UINT;
                break;
            case 0x13:
                element.Format = DXGI_FORMAT_R32G32_UINT;
                break;
            case 0x17:
                element.Format = DXGI_FORMAT_R32G32B32_UINT;
                break;
            case 0x1f:
                element.Format = DXGI_FORMAT_R32G32B32A32_UINT;
                break;
            case 0x21:
                element.Format = DXGI_FORMAT_R32_SINT;
                break;
            case 0x23:
                element.Format = DXGI_FORMAT_R32G32_SINT;
                break;
            case 0x27:
                element.Format = DXGI_FORMAT_R32G32B32_SINT;
                break;
            case 0x2f:
                element.Format = DXGI_FORMAT_R32G32B32A32_SINT;
                break;
            case 0x31:
                element.Format = DXGI_FORMAT_R32_FLOAT;
                break;
            case 0x33:
                element.Format = DXGI_FORMAT_R32G32_FLOAT;
                break;
            case 0x37:
                element.Format = DXGI_FORMAT_R32G32B32_FLOAT;
                break;
            case 0x3f:
                element.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
                break;
            default:
                logger.error("Input element has unsupported format");
        }
        input_elements.push_back(element);
    }

    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
    HRESULT result = device->CreateInputLayout(
        input_elements.data(), input_elements.size(), shader->get(),
        shader->getSize(), input_layout.GetAddressOf());

    if (FAILED(result)) {
        logger.warning("Input layout for vertex shader was not created");
        return nullptr;
    }

    return input_layout;
}
