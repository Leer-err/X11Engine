#include "Graphics.h"

#include "../Logger/Logger.h"
#include "../Window/Window.h"
#include "SwapChain.h"

Graphics::Graphics() {
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    ComPtr<IDXGIFactory2> factory;
    ComPtr<IDXGIAdapter1> adapter;

    ComPtr<ID3DBlob> vsBlob;

    HWND hWnd = Window::get()->GetHandle();
    int height = Window::get()->GetHeight();
    int width = Window::get()->GetWidth();

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_11_1};
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    FatalErrorIfFailed(
        CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory)),
        L"Failed to create DXGI factory");
    FatalErrorIfFailed(factory.As(&m_factory),
                       L"Failed to create DXGI factory");

    FatalErrorIfFailed(m_factory->EnumAdapters1(0, &adapter),
                       L"Failed to create DXGI adapter");
    FatalErrorIfFailed(adapter.As(&m_adapter),
                       L"Failed to create DXGI adapter");

    FatalErrorIfFailed(
        D3D11CreateDevice(m_adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr,
                          createDeviceFlags, &featureLevels[0],
                          numFeatureLevels, D3D11_SDK_VERSION, &device,
                          &m_featureLevel, &context),
        L"Failed to create Direct3D device");
    FatalErrorIfFailed(device.As(&m_device),
                       L"Failed to create Direct3D device");
    FatalErrorIfFailed(context.As(&m_context),
                       L"Failed to create Direct3D device context");

#ifdef _DEBUG
    LogIfFailed(m_device->QueryInterface(m_debug.GetAddressOf()),
                L"Failed to create debug device");
#endif

    m_states = std::make_unique<CommonStates>(m_device.Get());

    m_swapChain = std::make_unique<SwapChain>(width, height, hWnd,
                                              m_factory.Get(), m_device.Get());

    FatalErrorIfFailed(
        m_factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER),
        L"MakeWindowAssociation failed");

    ComPtr<ID3D11Texture2D> depth = CreateDepthStencil2D(
        DXGI_FORMAT_D24_UNORM_S8_UINT, false, width, height);
    m_depthStencilView = CreateTexture2DDSV(depth, 0);
    m_rtv = CreateTexture2DRTV({m_swapChain->GetBuffer(0)}, 0);

    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<FLOAT>(width);
    viewport.Height = static_cast<FLOAT>(height);
    viewport.MaxDepth = 1.0f;

    m_context->RSSetViewports(1, &viewport);

    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ComPtr<ID3D11DepthStencilState> depthState;

    D3D11_DEPTH_STENCIL_DESC depthDesc = {};
    depthDesc.DepthEnable = true;
    depthDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthDesc.StencilEnable = false;

    m_device->CreateDepthStencilState(&depthDesc, depthState.GetAddressOf());

    m_context->OMSetDepthStencilState(depthState.Get(), 0);

    m_CBVSModel = CreateConstantBuffer(
        true, nullptr, ((sizeof(CB_VS_PER_MODEL) >> 4) + 1) << 4);
    m_CBVSFrame = CreateConstantBuffer(
        true, nullptr, ((sizeof(CB_VS_PER_FRAME) >> 4) + 1) << 4);
    m_CBVSWindow = CreateConstantBuffer(
        true, nullptr, ((sizeof(CB_VS_PER_WINDOW) >> 4) + 1) << 4);
    m_CBPSFrame = CreateConstantBuffer(
        true, nullptr, ((sizeof(CB_PS_PER_FRAME) >> 4) + 1) << 4);
    m_boneBuffer =
        CreateStructuredBuffer(256, sizeof(matrix), true, false, nullptr);
    m_boneBufferSRV = CreateBufferSRV(m_boneBuffer, sizeof(matrix), 256);

    m_lightBuffer = CreateStructuredBuffer(
        MAX_POINT_LIGHTS, sizeof(Graphics::PointLight), true, false, nullptr);
    m_context->VSSetConstantBuffers(0, 1, m_CBVSWindow.GetAddressOf());
    m_context->VSSetConstantBuffers(1, 1, m_CBVSFrame.GetAddressOf());
    m_context->VSSetConstantBuffers(2, 1, m_CBVSModel.GetAddressOf());
    m_context->PSSetConstantBuffers(0, 1, m_CBPSFrame.GetAddressOf());
    m_context->VSSetShaderResources(0, 1, m_boneBufferSRV.GetAddressOf());

    UpdatePerWindowBuffers();
}

Graphics::~Graphics() {
    m_context->ClearState();
#ifdef _DEBUG
    m_debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif
}

void Graphics::Clear() {
    pointLights.clear();
    static constexpr FLOAT color[4] = {0.f, 0.f, 0.f, 0.f};
    m_context->OMSetRenderTargets(1, m_rtv.GetAddressOf(),
                                  m_depthStencilView.Get());

    m_context->ClearRenderTargetView(m_rtv.Get(), color);
    m_context->ClearDepthStencilView(m_depthStencilView.Get(),
                                     D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
                                     1.f, 0);
}

void Graphics::Present() { m_swapChain->Present(); }

void Graphics::Draw(const Mesh& mesh, const Material& mat) {
    UINT stride = sizeof(VertexSkinning);
    UINT offset = 0;

    m_renderMutex.lock();

    SetPixelShaderResources(mat.resources[PIXEL_SHADER_STAGE],
                            mat.pixelShader.lightsIndex);
    SetVertexShaderResources(mat.resources[VERTEX_SHADER_STAGE]);

    m_context->IASetIndexBuffer(mesh.indices.buffer.Get(), DXGI_FORMAT_R32_UINT,
                                0);
    m_context->IASetVertexBuffers(0, 1, mesh.vertices.GetAddressOf(), &stride,
                                  &offset);

    m_context->PSSetShader(mat.pixelShader.shader.Get(), nullptr, 0);
    m_context->VSSetShader(mat.vertexShader.shader.Get(), nullptr, 0);
    m_context->IASetInputLayout(mat.vertexShader.inputLayout.Get());

    m_context->DrawIndexed(mesh.indices.indexCount, 0, 0);
    m_renderMutex.unlock();
}

void Graphics::SetProjectionMatrix(const matrix& projection) {
    CB_VS_PER_WINDOW.projection = projection.Transpose();
}

void Graphics::SetViewMatrix(const matrix& viewMatrix,
                             const vector3& cameraPosition) {
    CB_VS_PER_FRAME.view = viewMatrix.Transpose();
    CB_PS_PER_FRAME.viewPos = cameraPosition;
}

void Graphics::SetWorldMatrix(const matrix& world) {
    CB_VS_PER_MODEL.world = world.Transpose();
    CB_VS_PER_MODEL.normalMatrix = world.Inverse();
}

void Graphics::SetDirLight(const DirLight& light) {
    CB_PS_PER_FRAME.dirLight.ambient = light.ambient;
    CB_PS_PER_FRAME.dirLight.diffuse = light.diffuse;
    CB_PS_PER_FRAME.dirLight.specular = light.specular;
    CB_PS_PER_FRAME.dirLight.direction = light.direction;
}

void Graphics::SetPointLight(const ::PointLight& light,
                             const vector3& position) {
    PointLight pointLight;
    pointLight.position = position;
    pointLight.ambient = light.ambient;
    pointLight.diffuse = light.diffuse;
    pointLight.specular = light.specular;
    pointLight.lin = light.lin;
    pointLight.quadratic = light.quadratic;
    pointLight.constant = light.constant;
    pointLights.push_back(pointLight);
}

void Graphics::SetBoneData(const vector<matrix>& boneData) {
    UpdateBuffer(m_boneBuffer, boneData.data(),
                 sizeof(matrix) * boneData.size());
}

void Graphics::UpdatePerFrameBuffers() {
    UpdateConstantBuffer(m_CBVSFrame, &CB_VS_PER_FRAME);
    UpdateConstantBuffer(m_CBPSFrame, &CB_PS_PER_FRAME);
    UpdateBuffer(m_lightBuffer, pointLights.data(),
                 sizeof(Graphics::PointLight) * pointLights.size());
}

void Graphics::UpdatePerModelBuffers() {
    UpdateConstantBuffer(m_CBVSModel, &CB_VS_PER_MODEL);
}

void Graphics::UpdatePerWindowBuffers() {
    UpdateConstantBuffer(m_CBVSWindow, &CB_VS_PER_WINDOW);
}

ComPtr<ID3D11Buffer> Graphics::CreateConstantBuffer(bool CPUWritable,
                                                    const void* data,
                                                    size_t dataSize) const {
    ComPtr<ID3D11Buffer> buf;

    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = dataSize;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    if (CPUWritable) {
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.Usage = D3D11_USAGE_DYNAMIC;
    } else {
        desc.CPUAccessFlags = NULL;
        desc.Usage = D3D11_USAGE_IMMUTABLE;
    }

    D3D11_SUBRESOURCE_DATA res = {};
    res.pSysMem = data;

    D3D11_SUBRESOURCE_DATA* pInitialData = data == nullptr ? nullptr : &res;

    LogIfFailed(m_device->CreateBuffer(&desc, pInitialData, buf.GetAddressOf()),
                L"Failed to create constant buffer");

    return buf;
}

ComPtr<ID3D11Buffer> Graphics::CreateTextureBuffer(bool CPUWritable,
                                                   const void* data,
                                                   size_t dataSize) const {
    ComPtr<ID3D11Buffer> buf;

    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = dataSize;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    if (CPUWritable) {
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.Usage = D3D11_USAGE_DYNAMIC;
    } else {
        desc.CPUAccessFlags = NULL;
        desc.Usage = D3D11_USAGE_IMMUTABLE;
    }

    D3D11_SUBRESOURCE_DATA res = {};
    res.pSysMem = data;

    D3D11_SUBRESOURCE_DATA* pInitialData = data == nullptr ? nullptr : &res;

    LogIfFailed(m_device->CreateBuffer(&desc, pInitialData, buf.GetAddressOf()),
                L"Failed to create texture buffer");

    return buf;
}

ComPtr<ID3D11Buffer> Graphics::CreateStructuredBuffer(UINT count,
                                                      UINT structureSize,
                                                      bool CPUWritable,
                                                      bool GPUWritable,
                                                      const void* data) const {
    ComPtr<ID3D11Buffer> buffer;
    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = count * structureSize;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.StructureByteStride = structureSize;
    desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

    D3D11_SUBRESOURCE_DATA subRes = {};
    subRes.pSysMem = data;

    D3D11_SUBRESOURCE_DATA* pInitialData = data == nullptr ? nullptr : &subRes;

    if (!CPUWritable && !GPUWritable) {
        desc.Usage = D3D11_USAGE_IMMUTABLE;
    } else if (CPUWritable && !GPUWritable) {
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    } else if (!CPUWritable && GPUWritable) {
        desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
        desc.Usage = D3D11_USAGE_DEFAULT;
    } else {
        return nullptr;
    }

    LogIfFailed(
        m_device->CreateBuffer(&desc, pInitialData, buffer.GetAddressOf()),
        L"Failed to create structured buffer");

    return buffer;
}

ComPtr<ID3D11Buffer> Graphics::CreateVertexBuffer(UINT size, bool dynamic,
                                                  bool streamout,
                                                  const void* data) const {
    ComPtr<ID3D11Buffer> buffer;
    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = size;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    if (dynamic && !streamout) {
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    } else if (!dynamic && streamout) {
        desc.BindFlags |= D3D11_BIND_STREAM_OUTPUT;
        desc.Usage = D3D11_USAGE_DEFAULT;
    } else if (!dynamic && !streamout) {
        desc.Usage = D3D11_USAGE_IMMUTABLE;
    } else {
        return nullptr;
    }

    D3D11_SUBRESOURCE_DATA subRes = {};
    subRes.pSysMem = data;

    D3D11_SUBRESOURCE_DATA* pInitialData = data == nullptr ? nullptr : &subRes;

    LogIfFailed(
        m_device->CreateBuffer(&desc, pInitialData, buffer.GetAddressOf()),
        L"Failed to create vertex buffer");

    return buffer;
}

IndexBuffer Graphics::CreateIndexBuffer(UINT size, bool dynamic,
                                        const void* data) const {
    ComPtr<ID3D11Buffer> buffer;
    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = size;
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    if (dynamic) {
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    } else {
        desc.Usage = D3D11_USAGE_IMMUTABLE;
    }

    D3D11_SUBRESOURCE_DATA subRes = {};
    subRes.pSysMem = data;

    D3D11_SUBRESOURCE_DATA* pInitialData = data == nullptr ? nullptr : &subRes;

    LogIfFailed(
        m_device->CreateBuffer(&desc, pInitialData, buffer.GetAddressOf()),
        L"Failed to create index buffer");

    return {buffer, size / sizeof(uint32_t)};
}

bool Graphics::UpdateConstantBuffer(const ComPtr<ID3D11Buffer>& buffer,
                                    const void* data) const {
    D3D11_MAPPED_SUBRESOURCE res;

    HRESULT hr =
        m_context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, NULL, &res);
    if (SUCCEEDED(hr)) {
        memcpy(res.pData, data, res.RowPitch);
        m_context->Unmap(buffer.Get(), 0);
        return true;
    } else {
        LogIfFailed(hr, L"Failed to map constant buffer %x", buffer);
        return false;
    }
}

bool Graphics::UpdateBuffer(const ComPtr<ID3D11Buffer>& buf, const void* data,
                            size_t size) const {
    D3D11_MAPPED_SUBRESOURCE res;

    HRESULT hr =
        m_context->Map(buf.Get(), 0, D3D11_MAP_WRITE_DISCARD, NULL, &res);
    if (SUCCEEDED(hr)) {
        memcpy(res.pData, data, size);
        m_context->Unmap(buf.Get(), 0);
        return true;
    } else {
        LogIfFailed(hr, L"Failed to map buffer %x", buf);
        return false;
    }
}

ComPtr<ID3D11Texture2D> Graphics::CreateTexture2D(DXGI_FORMAT format,
                                                  bool CPUWritable,
                                                  bool GPUWritable, int width,
                                                  int height,
                                                  const void* data) const {
    ComPtr<ID3D11Texture2D> texture;

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Format = format;
    desc.ArraySize = 1;
    desc.MipLevels = 1;
    desc.Width = width;
    desc.Height = height;
    desc.SampleDesc.Count = 1;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    if (!CPUWritable && !GPUWritable) {
        desc.Usage = D3D11_USAGE_IMMUTABLE;
    } else if (CPUWritable && !GPUWritable) {
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    } else if (!CPUWritable && GPUWritable) {
        desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
        desc.Usage = D3D11_USAGE_DEFAULT;
    } else {
        return nullptr;
    }

    D3D11_SUBRESOURCE_DATA subRes = {};
    subRes.pSysMem = data;
    subRes.SysMemPitch = width * 4;
    subRes.SysMemSlicePitch = width * height * 4;

    D3D11_SUBRESOURCE_DATA* pInitialData = data == nullptr ? nullptr : &subRes;

    LogIfFailed(m_device->CreateTexture2D(&desc, &subRes, &texture),
                L"Failed to create 2D texture");

    return texture;
}

ComPtr<ID3D11Texture2D> Graphics::CreateSkyboxTexture(
    DXGI_FORMAT format, bool CPUWritable, bool GPUWritable, int width,
    int height, array<const void*, 6> data) const {
    ComPtr<ID3D11Texture2D> texture;

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Format = format;
    desc.ArraySize = 6;
    desc.MipLevels = 1;
    desc.Width = width;
    desc.Height = height;
    desc.SampleDesc.Count = 1;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

    if (!CPUWritable && !GPUWritable) {
        desc.Usage = D3D11_USAGE_IMMUTABLE;
    } else if (CPUWritable && !GPUWritable) {
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    } else if (!CPUWritable && GPUWritable) {
        desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
        desc.Usage = D3D11_USAGE_DEFAULT;
    } else {
        return nullptr;
    }

    D3D11_SUBRESOURCE_DATA subRes[6] = {};
    for (int i = 0; i < 6; i++) {
        subRes[i].pSysMem = data[i];
        subRes[i].SysMemPitch = width * 4;
        subRes[i].SysMemSlicePitch = width * height * 4;
    }

    LogIfFailed(m_device->CreateTexture2D(&desc, subRes, &texture),
                L"Failed to create 2D texture");

    return texture;
}

ComPtr<ID3D11Texture2D> Graphics::CreateDepthStencil2D(DXGI_FORMAT format,
                                                       bool GPUReadable,
                                                       int width,
                                                       int height) const {
    ComPtr<ID3D11Texture2D> depth;

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Format = format;
    desc.ArraySize = 1;
    desc.MipLevels = 1;
    desc.Width = width;
    desc.Height = height;
    desc.SampleDesc.Count = 1;
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    if (GPUReadable) {
        desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
    }

    FatalErrorIfFailed(
        m_device->CreateTexture2D(&desc, nullptr, depth.GetAddressOf()),
        L"Failed to create 2D depth texture");

    return depth;
}

ComPtr<ID3D11DepthStencilView> Graphics::CreateTexture2DDSV(
    const ComPtr<ID3D11Resource>& res, UINT mip) const {
    ComPtr<ID3D11DepthStencilView> dsv;

    D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
    desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.Texture2D.MipSlice = mip;

    FatalErrorIfFailed(
        m_device->CreateDepthStencilView(res.Get(), &desc, dsv.GetAddressOf()),
        L"Failed to create 2D DSV for %x", res.Get());

    return dsv;
}

ComPtr<ID3D11RenderTargetView> Graphics::CreateTexture2DRTV(
    const ComPtr<ID3D11Resource>& res, UINT mip) const {
    ComPtr<ID3D11RenderTargetView> rtv;

    D3D11_RENDER_TARGET_VIEW_DESC desc = {};
    desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.Texture2D.MipSlice = mip;

    FatalErrorIfFailed(
        m_device->CreateRenderTargetView(res.Get(), &desc, rtv.GetAddressOf()),
        L"Failed to create 2D RTV for %x", res.Get());

    return rtv;
}

ComPtr<ID3D11ShaderResourceView> Graphics::CreateBufferSRV(
    const ComPtr<ID3D11Resource>& res, UINT elementSize,
    UINT numElements) const {
    ComPtr<ID3D11ShaderResourceView> srv;

    D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    desc.Buffer.ElementOffset = 0;
    desc.Buffer.ElementWidth = numElements;

    LogIfFailed(m_device->CreateShaderResourceView(res.Get(), &desc,
                                                   srv.GetAddressOf()),
                L"Failed to create buffer %x SRV", res.Get());

    return srv;
}

ComPtr<ID3D11ShaderResourceView> Graphics::CreateTexture2DSRV(
    const ComPtr<ID3D11Resource>& res, DXGI_FORMAT format) const {
    ComPtr<ID3D11ShaderResourceView> srv;
    D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
    desc.Format = format;
    desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    desc.Texture2D.MipLevels = 1;

    LogIfFailed(m_device->CreateShaderResourceView(res.Get(), &desc,
                                                   srv.GetAddressOf()),
                L"Failed to create 2D texture %x SRV", res.Get());

    return srv;
}

ComPtr<ID3D11InputLayout> Graphics::CreateInputLayoutFromShader(
    const ComPtr<ID3DBlob>& shaderBytecode) const {
    ComPtr<ID3D11InputLayout> inputLayout;

    ComPtr<ID3D11ShaderReflection> refl;
    D3D11_SHADER_DESC shaderDesc;
    vector<D3D11_INPUT_ELEMENT_DESC> inputElements;

    D3DReflect(shaderBytecode->GetBufferPointer(),
               shaderBytecode->GetBufferSize(),
               IID_PPV_ARGS(refl.GetAddressOf()));

    refl->GetDesc(&shaderDesc);

    for (UINT i = 0; i < shaderDesc.InputParameters; i++) {
        D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
        refl->GetInputParameterDesc(i, &paramDesc);

        D3D11_INPUT_ELEMENT_DESC elementDesc;
        elementDesc.SemanticName = paramDesc.SemanticName;
        elementDesc.SemanticIndex = paramDesc.SemanticIndex;
        elementDesc.InputSlot = 0;
        elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        elementDesc.InstanceDataStepRate = 0;

        if (paramDesc.Mask == 0x1) {
            if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
                elementDesc.Format = DXGI_FORMAT_R32_UINT;
            else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
                elementDesc.Format = DXGI_FORMAT_R32_SINT;
            else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
                elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
        } else if (paramDesc.Mask <= 0x3) {
            if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
                elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
            else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
                elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
            else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
                elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
        } else if (paramDesc.Mask <= 0x7) {
            if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
                elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
            else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
                elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
            else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
                elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
        } else if (paramDesc.Mask <= 0xf) {
            if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
                elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
            else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
                elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
            else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
                elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        }

        inputElements.push_back(elementDesc);
    }

    LogIfFailed(
        m_device->CreateInputLayout(inputElements.data(), inputElements.size(),
                                    shaderBytecode->GetBufferPointer(),
                                    shaderBytecode->GetBufferSize(),
                                    inputLayout.GetAddressOf()),
        L"Failed to create input layout");

    return inputLayout;
}

PixelShader Graphics::CreatePixelShader(
    const ComPtr<ID3DBlob>& shaderBytecode) const {
    PixelShader shader = {};
    ComPtr<ID3D11ShaderReflection> shaderReflection;
    D3D11_SHADER_DESC shaderDesc;

    HRESULT hr = m_device->CreatePixelShader(
        shaderBytecode->GetBufferPointer(), shaderBytecode->GetBufferSize(),
        nullptr, shader.shader.GetAddressOf());

    if (FAILED(hr)) {
        Logger::get()->Error(L"Failed to create pixel shader with %x", hr);
        return shader;
    }

    hr = D3DReflect(shaderBytecode->GetBufferPointer(),
                    shaderBytecode->GetBufferSize(),
                    IID_PPV_ARGS(shaderReflection.GetAddressOf()));

    if (FAILED(hr)) {
        Logger::get()->Error(
            L"Failed to create pixel shader reflection with %x", hr);
        shader.shader.Reset();
        return shader;
    }

    shaderReflection->GetDesc(&shaderDesc);
    for (int i = 0; i < shaderDesc.BoundResources; i++) {
        D3D11_SHADER_INPUT_BIND_DESC bindDesc;
        shaderReflection->GetResourceBindingDesc(i, &bindDesc);

        if (NULL == strcmp(bindDesc.Name, "baseColor")) {
            shader.baseColorIndex = bindDesc.BindPoint;
        }
        if (NULL == strcmp(bindDesc.Name, "specularTex")) {
            shader.specularIndex = bindDesc.BindPoint;
        }
        if (NULL == strcmp(bindDesc.Name, "emissionTex")) {
            shader.emissionIndex = bindDesc.BindPoint;
        }
        if (NULL == strcmp(bindDesc.Name, "pointLights")) {
            shader.lightsIndex = bindDesc.BindPoint;
        }
    }

    return shader;
}

VertexShader Graphics::CreateVertexShader(
    const ComPtr<ID3DBlob>& shaderBytecode) const {
    VertexShader shader;

    LogIfFailed(
        m_device->CreateVertexShader(shaderBytecode->GetBufferPointer(),
                                     shaderBytecode->GetBufferSize(), nullptr,
                                     shader.shader.GetAddressOf()),
        L"Failed to create vertex shader");

    shader.inputLayout = CreateInputLayoutFromShader(shaderBytecode);

    if (shader.inputLayout.Get() == nullptr) {
        shader.shader.Reset();
    }

    return shader;
}

void Graphics::DrawSkybox() const {
    ComPtr<ID3D11ShaderResourceView> skybox;

    UINT stride = sizeof(vector3);
    UINT offset = 0;

    m_context->PSSetShader(m_skyboxPS.shader.Get(), nullptr, 0);
    m_context->VSSetShader(m_skyboxVS.shader.Get(), nullptr, 0);

    ID3D11SamplerState* sampler = m_states->LinearClamp();
    m_context->PSSetSamplers(0, 1, &sampler);

    m_device->CreateShaderResourceView(m_skybox.Get(), nullptr,
                                       skybox.GetAddressOf());
    m_context->PSSetShaderResources(0, 1, skybox.GetAddressOf());
    m_context->IASetIndexBuffer(m_skyboxIndices.buffer.Get(),
                                DXGI_FORMAT_R32_UINT, 0);
    m_context->IASetVertexBuffers(0, 1, m_skyboxVertices.GetAddressOf(),
                                  &stride, &offset);
    m_context->IASetInputLayout(m_skyboxVS.inputLayout.Get());
    m_context->DrawIndexed(m_skyboxIndices.indexCount, 0, 0);
}

void Graphics::SetSkyboxMesh() {
    vector3 vertices[] = {
        {-0.5f, -0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f}, {0.5f, 0.5f, -0.5f},
        {0.5f, -0.5f, -0.5f},  {-0.5f, -0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f},
        {0.5f, 0.5f, 0.5f},    {0.5f, -0.5f, 0.5f},
    };

    uint32_t indices[] = {0, 2, 1, 0, 3, 2, 3, 6, 2, 3, 7, 6, 7, 5, 6, 7, 4, 5,
                          4, 1, 5, 4, 0, 1, 1, 2, 6, 1, 6, 5, 0, 7, 3, 0, 4, 7};

    m_skyboxVertices =
        CreateVertexBuffer(sizeof(vector3) * 8, false, false, vertices);
    m_skyboxIndices = CreateIndexBuffer(sizeof(uint32_t) * 36, false, indices);
}

void Graphics::SetPixelShaderResources(const ShaderResources& res,
                                       int lightsIndex) {
    ComPtr<ID3D11ShaderResourceView>
        resources[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};
    for (int i = 0; i < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT; i++) {
        if (res.textures[i] != nullptr) {
            resources[i] = CreateTexture2DSRV(res.textures[i].Get());
        }
    }
    resources[lightsIndex] = CreateBufferSRV(
        m_lightBuffer.Get(), sizeof(Graphics::PointLight), pointLights.size());

    ID3D11ShaderResourceView*
        shaderResources[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};
    for (int i = 0; i < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT; i++) {
        shaderResources[i] = resources[i].Get();
    }
    m_context->PSSetShaderResources(
        0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, shaderResources);

    ID3D11SamplerState*
        samplerStates[D3D11_COMMONSHADER_SAMPLER_REGISTER_COUNT] = {};
    for (int i = 0; i < D3D11_COMMONSHADER_SAMPLER_REGISTER_COUNT; i++) {
        samplerStates[i] = res.samplers[i].Get();
    }
    samplerStates[0] = m_states->LinearClamp();
    m_context->PSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_REGISTER_COUNT,
                             samplerStates);
}

void Graphics::SetVertexShaderResources(const ShaderResources& res) {
    ComPtr<ID3D11ShaderResourceView>
        resources[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};
    for (int i = 0; i < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT; i++) {
        if (res.textures[i] != nullptr) {
            resources[i] = CreateTexture2DSRV(res.textures[i].Get());
        }
    }

    ID3D11ShaderResourceView*
        shaderResources[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};
    for (int i = 0; i < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT; i++) {
        shaderResources[i] = resources[i].Get();
    }
    m_context->VSSetShaderResources(
        1, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1, shaderResources);

    ID3D11SamplerState*
        samplerStates[D3D11_COMMONSHADER_SAMPLER_REGISTER_COUNT] = {};
    for (int i = 0; i < D3D11_COMMONSHADER_SAMPLER_REGISTER_COUNT; i++) {
        samplerStates[i] = res.samplers[i].Get();
    }
    m_context->VSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_REGISTER_COUNT,
                             samplerStates);
}