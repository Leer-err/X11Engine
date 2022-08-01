#include "Graphics.h"
#include "SwapChain.h"
#include "Logger/Logger.h"
#include "Window.h"

Graphics::Graphics()
{
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
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	if (SUCCEEDED(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory))) 
		&& SUCCEEDED(factory.As(&m_factory))) {
		Logger::get()->Debug(L"DXGI Factory created successfully");
	}
	else {
		Logger::get()->Error(L"DXGI Factory creation failed");
		Window::get()->Terminate();
	}

	if (SUCCEEDED(m_factory->EnumAdapters1(0, &adapter))
		&& SUCCEEDED(adapter.As(&m_adapter))) {
		Logger::get()->Debug(L"Adapter created successfully");
	}
	else {
		Logger::get()->Error(L"Adapter creation failed");
		Window::get()->Terminate();
	}

	if (SUCCEEDED(D3D11CreateDevice(m_adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr, createDeviceFlags, &featureLevels[0], numFeatureLevels,
		D3D11_SDK_VERSION, &device, &m_featureLevel, &context))
		&& SUCCEEDED(device.As(&m_device))
		&& SUCCEEDED(context.As(&m_context))) {
		Logger::get()->Debug(L"Device and context created successfully");
	}
	else {
		Logger::get()->Error(L"Device and context creation failed");
		Window::get()->Terminate();
	}

#ifdef _DEBUG
	m_device->QueryInterface(m_debug.GetAddressOf());
#endif

	m_swapChain = std::make_unique<SwapChain>(width, height, hWnd, m_factory.Get(), m_device.Get());

	m_factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);

	ID3D11Texture2D1* backBuffer = m_swapChain->GetBuffer(0);
	m_device->CreateRenderTargetView1(backBuffer, nullptr, &m_rtv);

	D3D11_VIEWPORT viewport = {};
	viewport.Width = width;
	viewport.Height = height;
	viewport.MaxDepth = 1.0f;

	m_context->RSSetViewports(1, &viewport);

	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ComPtr<ID3D11Texture2D> depthStencil;
	CD3D11_TEXTURE2D_DESC dsvDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, 1920, 1080);
	dsvDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	m_device->CreateTexture2D(&dsvDesc, NULL, &depthStencil);
	m_device->CreateDepthStencilView(depthStencil.Get(), NULL, &m_depthStencilView);

	m_context->OMSetDepthStencilState(nullptr, 0);

	m_CBVSModel = CreateConstantBuffer(true, nullptr, (sizeof(CB_VS_PER_MODEL) >> 4) + 1 << 4);
	m_CBVSFrame = CreateConstantBuffer(true, nullptr, (sizeof(CB_VS_PER_FRAME) >> 4) + 1 << 4);
	m_CBVSWindow = CreateConstantBuffer(true, nullptr, (sizeof(CB_VS_PER_WINDOW) >> 4) + 1 << 4);
	m_CBPSFrame = CreateConstantBuffer(true, nullptr, (sizeof(CB_PS_PER_FRAME) >> 4) + 1 << 4);
	m_lightBuffer = CreateStructuredBuffer(MAX_POINT_LIGHTS, sizeof(Graphics::PointLight), true, false, nullptr);
	m_context->VSSetConstantBuffers(0, 1, m_CBVSWindow.GetAddressOf());
	m_context->VSSetConstantBuffers(1, 1, m_CBVSFrame.GetAddressOf());
	m_context->VSSetConstantBuffers(2, 1, m_CBVSModel.GetAddressOf());
	m_context->PSSetConstantBuffers(0, 1, m_CBPSFrame.GetAddressOf());

	m_vertexBuffer = CreateBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, nullptr, sizeof(vertex) * VERTEX_BUFFER_SIZE);
	m_indexBuffer = CreateBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, nullptr, sizeof(uint32_t) * INDEX_BUFFER_SIZE);
	UINT stride = sizeof(vertex);
	UINT offset = 0;
	m_context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	m_context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	SetProjectionMatrix();
	UpdatePerWindowBuffers();
}

Graphics::~Graphics()
{
	m_context->ClearState();
#ifdef _DEBUG
	m_debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif
}

void Graphics::Clear()
{
	pointLights.clear();
	static constexpr FLOAT color[4] = { 0.f,0.f,0.f,0.f };
	m_context->OMSetRenderTargets(1, reinterpret_cast<ID3D11RenderTargetView**>(m_rtv.GetAddressOf()), m_depthStencilView.Get());

	m_context->ClearRenderTargetView(m_rtv.Get(), color);
	m_context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}

void Graphics::Present()
{
	m_swapChain->Present();
}

void Graphics::Draw(const Model* model)
{
	for (const auto& mesh : model->meshes) {
		UpdateBuffer(m_vertexBuffer, mesh.vertices.data(), mesh.vertices.size() * sizeof(vertex));
		UpdateBuffer(m_indexBuffer, mesh.indices.data(), mesh.indices.size() * sizeof(uint32_t));

		m_render_mutex.lock();

		m_context->PSSetShader(model->materials[mesh.materialIndex].pixelShader.Get(), nullptr, 0);
		m_context->VSSetShader(model->materials[mesh.materialIndex].vertexShader.Get(), nullptr, 0);
		m_context->IASetInputLayout(model->materials[mesh.materialIndex].inputLayout.Get());

		m_context->PSSetShaderResources(0, 1, model->materials[mesh.materialIndex].baseColor.GetAddressOf());
		m_context->PSSetShaderResources(1, 1, model->materials[mesh.materialIndex].diffuse.GetAddressOf());
		m_context->PSSetShaderResources(2, 1, model->materials[mesh.materialIndex].specular.GetAddressOf());
		m_context->PSSetShaderResources(3, 1, model->materials[mesh.materialIndex].emission.GetAddressOf());
		m_context->PSSetSamplers(0, 1, m_sampler.GetAddressOf());

		m_context->DrawIndexed(mesh.indices.size(), 0, 0);
		m_render_mutex.unlock();
	}
}

void Graphics::SetProjectionMatrix()
{
	int width = Window::get()->GetWidth();
	int height = Window::get()->GetHeight();
	CB_VS_PER_WINDOW.projection = PerspectiveProjectionMatrix((float)width / height, 60.f / 180.f * 3.14f, 1000.f, 0.01f).Transpose();
}

void Graphics::SetViewMatrix(const quaternion& viewRotation, const vector3& cameraPosition)
{
	CB_VS_PER_FRAME.view = LookToMatrix(cameraPosition, vector3(0.f, 0.f, 1.f).rotate(viewRotation), vector3(0.f, 1.f, 0.f).rotate(viewRotation)).Transpose();
	CB_PS_PER_FRAME.viewPos = cameraPosition;
}

void Graphics::SetWorldMatrix(const matrix& world)
{
	CB_VS_PER_MODEL.world = world.Transpose();
	CB_VS_PER_MODEL.normalMatrix = world.Inverse();
}

void Graphics::SetDirLight(const DirLight& light)
{
	CB_PS_PER_FRAME.dirLight.ambient = light.ambient;
	CB_PS_PER_FRAME.dirLight.diffuse = light.diffuse;
	CB_PS_PER_FRAME.dirLight.specular = light.specular;
	CB_PS_PER_FRAME.dirLight.direction = light.direction;
}

void Graphics::SetPointLight(const ::PointLight& light, const vector3& position)
{
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

void Graphics::UpdatePerFrameBuffers()
{
	UpdateConstantBuffer(m_CBVSFrame.Get(), &CB_VS_PER_FRAME);
	UpdateConstantBuffer(m_CBPSFrame.Get(), &CB_PS_PER_FRAME);
	UpdateBuffer(m_lightBuffer, pointLights.data());
	ComPtr<ID3D11ShaderResourceView> res = CreateBufferSRV(m_lightBuffer.Get(), sizeof(Graphics::PointLight), pointLights.size());
	m_context->PSSetShaderResources(4, 1, res.GetAddressOf());
}

void Graphics::UpdatePerModelBuffers()
{
	UpdateConstantBuffer(m_CBVSModel.Get(), &CB_VS_PER_MODEL);
}

void Graphics::UpdatePerWindowBuffers()
{
	UpdateConstantBuffer(m_CBVSWindow.Get(), &CB_VS_PER_WINDOW);
}

ComPtr<ID3D11Buffer> Graphics::CreateConstantBuffer(bool CPUWritable, const void* data, size_t dataSize) const
{
	ID3D11Buffer* buf;

	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = dataSize;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	if (CPUWritable) {
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.Usage = D3D11_USAGE_DYNAMIC;
	}
	else {
		desc.CPUAccessFlags = NULL;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
	}

	D3D11_SUBRESOURCE_DATA res = {};
	res.pSysMem = data;

	D3D11_SUBRESOURCE_DATA* pInitialData = data == nullptr ? nullptr : &res;

	m_device->CreateBuffer(&desc, pInitialData, &buf);

	return buf;
}

ComPtr<ID3D11Buffer> Graphics::CreateBuffer(D3D11_USAGE usage, D3D11_BIND_FLAG bind, const void* data, size_t dataSize) const
{
	ComPtr<ID3D11Buffer> buf;

	D3D11_BUFFER_DESC desc = {};
	desc.Usage = usage;
	desc.ByteWidth = dataSize;
	desc.BindFlags = bind;
	desc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA res = {};
	res.pSysMem = data;

	D3D11_SUBRESOURCE_DATA* pInitialData = data == nullptr ? nullptr : &res;

	m_device->CreateBuffer(&desc, pInitialData, &buf);

	return buf;
}

ComPtr<ID3D11Buffer> Graphics::CreateStructuredBuffer(UINT count, UINT structureSize, bool CPUWritable, bool GPUWritable, const void* data) const
{
	ID3D11Buffer* buffer;
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
		desc.CPUAccessFlags = 0;
	}
	if (CPUWritable && !GPUWritable) {
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	if (!CPUWritable && GPUWritable) {
		desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;
	}
	if (CPUWritable && GPUWritable) {
		return nullptr;
	}

	m_device->CreateBuffer(&desc, pInitialData, &buffer);

	return buffer;
}

void Graphics::UpdateConstantBuffer(ID3D11Buffer* buffer, const void* data) const
{
	D3D11_MAPPED_SUBRESOURCE res;

	m_context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &res);
	memcpy(res.pData, data, res.RowPitch);
	m_context->Unmap(buffer, 0);
}

void Graphics::UpdateBuffer(const ComPtr<ID3D11Buffer>& buf, const void* data, size_t size) const
{
	CD3D11_BOX box(0, 0, 0, size, 1, 1);
	const D3D11_BOX* pbox = size == 0 ? nullptr : &box;
	m_context->UpdateSubresource(buf.Get(), 0, pbox, data, 0, 0);
}

ComPtr<ID3D11ShaderResourceView> Graphics::CreateShaderResource(DXGI_FORMAT format, int width, int height, const void* pData) const
{
	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11ShaderResourceView> resource;

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Format = format;
	desc.ArraySize = 1;
	desc.MipLevels = 1;
	desc.Width = width;
	desc.Height = height;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA res = {};
	res.pSysMem = pData;
	res.SysMemPitch = width * 4;
	res.SysMemSlicePitch = width * height * 4;

	m_device->CreateTexture2D(&desc, &res, texture.GetAddressOf());
	m_device->CreateShaderResourceView(texture.Get(), nullptr, resource.GetAddressOf());

	return resource;
}

ComPtr<ID3D11ShaderResourceView> Graphics::CreateBufferSRV(ID3D11Resource* res, UINT elementSize, UINT numElements) const
{
	ID3D11ShaderResourceView* srv;
	D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = numElements;

	m_device->CreateShaderResourceView(res, &desc, &srv);

	return srv;
}

ComPtr<ID3D11InputLayout> Graphics::CreateInputLayoutFromShader(ComPtr<ID3DBlob> shaderBytecode)
{
	ID3D11InputLayout* inputLayout;

	ID3D11ShaderReflection* refl;
	D3D11_SHADER_DESC shaderDesc;
	vector<D3D11_INPUT_ELEMENT_DESC> inputElements;

	D3DReflect(shaderBytecode->GetBufferPointer(), shaderBytecode->GetBufferSize(), IID_PPV_ARGS(&refl));

	refl->GetDesc(&shaderDesc);

	for (int i = 0; i < shaderDesc.InputParameters; i++) {
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		refl->GetInputParameterDesc(i, &paramDesc);

		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if (paramDesc.Mask <= 3)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (paramDesc.Mask <= 7)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (paramDesc.Mask <= 15)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		inputElements.push_back(elementDesc);
	}

	m_device->CreateInputLayout(inputElements.data(), inputElements.size(), shaderBytecode->GetBufferPointer(), shaderBytecode->GetBufferSize(), &inputLayout);

	return inputLayout;
}

ComPtr<ID3D11PixelShader> Graphics::CreatePixelShader(ComPtr<ID3DBlob> shaderBytecode)
{
	ComPtr<ID3D11PixelShader> shader;

	m_device->CreatePixelShader(shaderBytecode->GetBufferPointer(), shaderBytecode->GetBufferSize(), nullptr, &shader);

	return shader;
}

ComPtr<ID3D11VertexShader> Graphics::CreateVertexShader(ComPtr<ID3DBlob> shaderBytecode)
{
	ComPtr<ID3D11VertexShader> shader;

	m_device->CreateVertexShader(shaderBytecode->GetBufferPointer(), shaderBytecode->GetBufferSize(), nullptr, &shader);

	return shader;
}
