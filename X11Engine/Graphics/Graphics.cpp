#include "Graphics.h"
#include "SwapChain.h"
#include "Buffer.h"

Graphics::Graphics(UINT width, UINT height, HWND hWnd)
{
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;
	ComPtr<IDXGIFactory2> factory;
	ComPtr<IDXGIAdapter1> adapter;

	ComPtr<ID3DBlob> vsBlob;
	ComPtr<ID3DBlob> psBlob;
	ComPtr<ID3DBlob> errorBlob;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	ThrowIfFailed(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory)));
	factory.As(&m_factory);
	ThrowIfFailed(m_factory->EnumAdapters1(0, &adapter));
	adapter.As(&m_adapter);
	ThrowIfFailed(D3D11CreateDevice(m_adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr, createDeviceFlags, &featureLevels[0], numFeatureLevels,
		D3D11_SDK_VERSION, &device, &m_featureLevel, &context));

	device.As(&m_device);
	context.As(&m_context);

	m_swapChain = std::make_unique<SwapChain>(width, height, hWnd, m_factory.Get(), m_device.Get());

	m_factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);

	ID3D11Texture2D1* backBuffer = m_swapChain->GetBuffer(0);

	m_device->CreateRenderTargetView1(backBuffer, nullptr, &m_rtv);

	D3D11_VIEWPORT viewport = {};
	viewport.Width = width;
	viewport.Height = height;
	viewport.MaxDepth = 1.0f;

	m_context->RSSetViewports(1, &viewport);

	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ThrowIfFailed(D3DCompileFromFile(L"PixelShader.hlsl", nullptr, nullptr, "main", "ps_5_0", shaderFlags, 0, &psBlob, &errorBlob));
	ThrowIfFailed(D3DCompileFromFile(L"VertexShader.hlsl", nullptr, nullptr, "main", "vs_5_0", shaderFlags, 0, &vsBlob, &errorBlob));

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	ThrowIfFailed(m_device->CreateInputLayout(&layout[0], 1, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_inputLayout));
	m_context->IASetInputLayout(m_inputLayout.Get());
	ThrowIfFailed(m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_pixelShader));
	ThrowIfFailed(m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vertexShader));
	m_context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	m_context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

Graphics::~Graphics()
{
	m_context->ClearState();
}

void Graphics::PreFrame(CameraComponent* camera)
{
	constexpr FLOAT color[4] = { 0.f,0.f,0.f,0.f };
	m_context->OMSetRenderTargets(1, reinterpret_cast<ID3D11RenderTargetView**>(m_rtv.GetAddressOf()), nullptr);
	m_context->ClearRenderTargetView(m_rtv.Get(), &color[0]);

	matrix pr = camera->projectionMatrix.Transpose();
	const void* p = &pr;
	Buffer constantBuffer{ D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, m_device.Get(), p, sizeof(float) * 16 };
	m_context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddress());
}

void Graphics::PostFrame()
{
	m_swapChain->Present();
}

void Graphics::Draw(const vector<vector3>& vertices, const vector<uint32_t>& indices)
{
	Buffer vertexBuffer = { D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, m_device.Get(), vertices.data(), sizeof(vector3) * vertices.size()};
	Buffer indexBuffer = { D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, m_device.Get(), indices.data(), sizeof(uint32_t) * indices.size()};

	UINT stride = sizeof(FLOAT) * 3;
	UINT offset = 0;

	m_render_mutex.lock();
	m_context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddress(), &stride, &offset);
	m_context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	m_context->DrawIndexed(indices.size(), 0, 0);
	m_render_mutex.unlock();
}
