#include "Graphics.h"

#include "Logger/Logger.h"
#include "SwapChain.h"
#include "Window.h"

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

    m_swapChain = std::make_unique<SwapChain>(width, height, hWnd,
                                              m_factory.Get(), m_device.Get());

    FatalErrorIfFailed(
        m_factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER),
        L"MakeWindowAssociation failed");

    ComPtr<ID3D11Texture2D> depth = CreateDepthStencil2D(
        DXGI_FORMAT_D24_UNORM_S8_UINT, false, width, height);
    m_depthStencilView = CreateTexture2DDSV(depth, 0);
    m_rtv = CreateTexture2DRTV(m_swapChain->GetBuffer(0), 0);

    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<FLOAT>(width);
    viewport.Height = static_cast<FLOAT>(height);
    viewport.MaxDepth = 1.0f;

    m_context->RSSetViewports(1, &viewport);

    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    m_context->OMSetDepthStencilState(nullptr, 0);

    m_CBVSModel = CreateConstantBuffer(
        true, nullptr, ((sizeof(CB_VS_PER_MODEL) >> 4) + 1) << 4);
    m_CBVSFrame = CreateConstantBuffer(
        true, nullptr, ((sizeof(CB_VS_PER_FRAME) >> 4) + 1) << 4);
    m_CBVSWindow = CreateConstantBuffer(
        true, nullptr, ((sizeof(CB_VS_PER_WINDOW) >> 4) + 1) << 4);
    m_CBPSFrame = CreateConstantBuffer(
        true, nullptr, ((sizeof(CB_PS_PER_FRAME) >> 4) + 1) << 4);
    m_lightBuffer = CreateStructuredBuffer(
        MAX_POINT_LIGHTS, sizeof(Graphics::PointLight), true, false, nullptr);
    m_context->VSSetConstantBuffers(0, 1, m_CBVSWindow.GetAddressOf());
    m_context->VSSetConstantBuffers(1, 1, m_CBVSFrame.GetAddressOf());
    m_context->VSSetConstantBuffers(2, 1, m_CBVSModel.GetAddressOf());
    m_context->PSSetConstantBuffers(0, 1, m_CBPSFrame.GetAddressOf());

    SetProjectionMatrix();
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

void Graphics::Draw(const Model* model) {
    for (const auto& mesh : model->meshes) {
        UINT stride = sizeof(vertex);
        UINT offset = 0;

        m_renderMutex.lock();

        m_context->IASetIndexBuffer(mesh.indices.buffer.Get(),
                                    DXGI_FORMAT_R32_UINT, 0);
        m_context->IASetVertexBuffers(0, 1, mesh.vertices.GetAddressOf(),
                                      &stride, &offset);

        m_context->PSSetShader(
            model->materials[mesh.materialIndex].pixelShader.Get(), nullptr, 0);
        m_context->VSSetShader(
            model->materials[mesh.materialIndex].vertexShader.Get(), nullptr,
            0);
        m_context->IASetInputLayout(
            model->materials[mesh.materialIndex].inputLayout.Get());

        ID3D11ShaderResourceView*
            shaderResources[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};
        shaderResources[0] =
            CreateTexture2DSRV(
                model->materials[mesh.materialIndex].baseColor.Get())
                .Get();
        shaderResources[1] =
            CreateTexture2DSRV(
                model->materials[mesh.materialIndex].diffuse.Get())
                .Get();
        shaderResources[2] =
            CreateTexture2DSRV(
                model->materials[mesh.materialIndex].specular.Get())
                .Get();
        shaderResources[3] =
            CreateTexture2DSRV(
                model->materials[mesh.materialIndex].emission.Get())
                .Get();
        shaderResources[4] =
            CreateBufferSRV(m_lightBuffer.Get(), sizeof(Graphics::PointLight),
                            pointLights.size())
                .Get();

        m_context->PSSetShaderResources(
            0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, shaderResources);
        m_context->PSSetSamplers(0, 1, m_sampler.GetAddressOf());

        m_context->DrawIndexed(mesh.indices.indexCount, 0, 0);
        m_renderMutex.unlock();
    }
}

void Graphics::SetProjectionMatrix() {
    int width = Window::get()->GetWidth();
    int height = Window::get()->GetHeight();
    CB_VS_PER_WINDOW.projection =
        PerspectiveProjectionMatrix((float)width / height, 60.f / 180.f * 3.14f,
                                    1000.f, 0.01f)
            .Transpose();
}

void Graphics::SetViewMatrix(const quaternion& viewRotation,
                             const vector3& cameraPosition) {
    CB_VS_PER_FRAME.view =
        LookToMatrix(cameraPosition,
                     vector3(0.f, 0.f, 1.f).rotate(viewRotation),
                     vector3(0.f, 1.f, 0.f).rotate(viewRotation))
            .Transpose();
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

void Graphics::UpdatePerFrameBuffers() {
    UpdateConstantBuffer(m_CBVSFrame.Get(), &CB_VS_PER_FRAME);
    UpdateConstantBuffer(m_CBPSFrame.Get(), &CB_PS_PER_FRAME);
    UpdateBuffer(m_lightBuffer.Get(), pointLights.data(),
                 sizeof(Graphics::PointLight) * pointLights.size());
}

void Graphics::UpdatePerModelBuffers() {
    UpdateConstantBuffer(m_CBVSModel.Get(), &CB_VS_PER_MODEL);
}

void Graphics::UpdatePerWindowBuffers() {
    UpdateConstantBuffer(m_CBVSWindow.Get(), &CB_VS_PER_WINDOW);
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
    }
    if (CPUWritable && !GPUWritable) {
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    }
    if (!CPUWritable && GPUWritable) {
        desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
        desc.Usage = D3D11_USAGE_DEFAULT;
    }
    if (CPUWritable && GPUWritable) {
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

    return {buffer, size};
}

bool Graphics::UpdateConstantBuffer(ComPtr<ID3D11Buffer> buffer,
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

bool Graphics::UpdateBuffer(ComPtr<ID3D11Buffer> buf, const void* data,
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
    }
    if (CPUWritable && !GPUWritable) {
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    }
    if (!CPUWritable && GPUWritable) {
        desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
        desc.Usage = D3D11_USAGE_DEFAULT;
    }
    if (CPUWritable && GPUWritable) {
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

ComPtr<ID3D11Texture2D> Graphics::CreateDepthStencil2D(DXGI_FORMAT format,
                                                       bool GPUReadable,
                                                       int width,
                                                       int height) const {
    ID3D11Texture2D* depth;

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

    FatalErrorIfFailed(m_device->CreateTexture2D(&desc, nullptr, &depth),
                       L"Failed to create 2D depth texture");

    return depth;
}

ComPtr<ID3D11DepthStencilView> Graphics::CreateTexture2DDSV(
    ComPtr<ID3D11Resource> res, UINT mip) const {
    ID3D11DepthStencilView* dsv;

    D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
    desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.Texture2D.MipSlice = mip;

    FatalErrorIfFailed(m_device->CreateDepthStencilView(res.Get(), &desc, &dsv),
                       L"Failed to create 2D DSV for %x", res.Get());

    return dsv;
}

ComPtr<ID3D11RenderTargetView> Graphics::CreateTexture2DRTV(
    ComPtr<ID3D11Resource> res, UINT mip) const {
    ID3D11RenderTargetView* rtv;

    D3D11_RENDER_TARGET_VIEW_DESC desc = {};
    desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.Texture2D.MipSlice = mip;

    FatalErrorIfFailed(m_device->CreateRenderTargetView(res.Get(), &desc, &rtv),
                       L"Failed to create 2D RTV for %x", res.Get());

    return rtv;
}

ComPtr<ID3D11ShaderResourceView> Graphics::CreateBufferSRV(
    ComPtr<ID3D11Resource> res, UINT elementSize, UINT numElements) const {
    ID3D11ShaderResourceView* srv;
    D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    desc.Buffer.ElementOffset = 0;
    desc.Buffer.ElementWidth = numElements;

    LogIfFailed(m_device->CreateShaderResourceView(res.Get(), &desc, &srv),
                L"Failed to create buffer %x SRV", res.Get());

    return srv;
}

ComPtr<ID3D11ShaderResourceView> Graphics::CreateTexture2DSRV(
    ComPtr<ID3D11Resource> res, DXGI_FORMAT format) const {
    ID3D11ShaderResourceView* srv;
    D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
    desc.Format = format;
    desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    desc.Texture2D.MipLevels = 1;

    LogIfFailed(m_device->CreateShaderResourceView(res.Get(), &desc, &srv),
                L"Failed to create 2D texture %x SRV", res.Get());

    return srv;
}

ComPtr<ID3D11InputLayout> Graphics::CreateInputLayoutFromShader(
    ComPtr<ID3DBlob> shaderBytecode) {
    ComPtr<ID3D11InputLayout> inputLayout;

    ID3D11ShaderReflection* refl;
    D3D11_SHADER_DESC shaderDesc;
    vector<D3D11_INPUT_ELEMENT_DESC> inputElements;

    D3DReflect(shaderBytecode->GetBufferPointer(),
               shaderBytecode->GetBufferSize(), IID_PPV_ARGS(&refl));

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

        if (paramDesc.Mask == 1) {
            if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
                elementDesc.Format = DXGI_FORMAT_R32_UINT;
            else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
                elementDesc.Format = DXGI_FORMAT_R32_SINT;
            else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
                elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
        } else if (paramDesc.Mask <= 3) {
            if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
                elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
            else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
                elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
            else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
                elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
        } else if (paramDesc.Mask <= 7) {
            if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
                elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
            else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
                elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
            else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
                elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
        } else if (paramDesc.Mask <= 15) {
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
        L"Failed to create input layout for shader");

    return inputLayout;
}

ComPtr<ID3D11PixelShader> Graphics::CreatePixelShader(
    ComPtr<ID3DBlob> shaderBytecode) {
    ComPtr<ID3D11PixelShader> shader;

    LogIfFailed(m_device->CreatePixelShader(shaderBytecode->GetBufferPointer(),
                                            shaderBytecode->GetBufferSize(),
                                            nullptr, shader.GetAddressOf()),
                L"Failed to create pixel shader");

    return shader;
}

ComPtr<ID3D11VertexShader> Graphics::CreateVertexShader(
    ComPtr<ID3DBlob> shaderBytecode) {
    ComPtr<ID3D11VertexShader> shader;

    LogIfFailed(m_device->CreateVertexShader(shaderBytecode->GetBufferPointer(),
                                             shaderBytecode->GetBufferSize(),
                                             nullptr, shader.GetAddressOf()),
                L"Failed to create vertex shader");

    return shader;
}
