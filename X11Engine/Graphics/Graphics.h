#pragma once
#include <d3d11_4.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <memory>
#include <mutex>
#include "Helper.h"
#include <vector>
#include "Model.h"

#include "Components/CameraComponent.h"

using Microsoft::WRL::ComPtr;
using std::unique_ptr;
using std::vector;
using std::mutex;
using std::make_unique;

class SwapChain;

constexpr uint32_t VERTEX_BUFFER_SIZE = 60000;
constexpr uint32_t INDEX_BUFFER_SIZE = 100000;

class Graphics
{
	struct {
		matrix projection;
	} CB_VS_PER_WINDOW;

	struct {
		matrix view;
	} CB_VS_PER_FRAME;

	struct {
		matrix world;
		matrix normalMatrix;
	} CB_VS_PER_MODEL;

	struct {
		vector3 ambientColor;
		char p0[4];
		vector3 diffuseColor;
		char p1[4];
		vector3 specularColor;
		char p2[4];
		vector3 lightPos;
		char p3[4];
		vector3 viewPos;
	} CB_PS_PER_FRAME; // char's here just for 16 byte memory alignment
public:
	static Graphics& get() {
		static Graphics instance;
		return instance;
	}

	void Clear();
	void Present();
	void Draw(const Model& model);

	void SetProjectionMatrix();
	void SetViewMatrix(const quaternion& viewDirection, const vector3& cameraPosition);
	void SetWorldMatrix(const matrix& world);
	void SetAmbientColor(vector3 color);
	void SetLight(vector3 pos, vector3 color);

	void UpdatePerFrameBuffers();
	void UpdatePerModelBuffers();
	void UpdatePerWindowBuffers();

	ComPtr<ID3D11Buffer> CreateBuffer(D3D11_USAGE usage, D3D11_BIND_FLAG bind, const void* data, size_t dataSize) const;
	void UpdateBuffer(const ComPtr<ID3D11Buffer>& buf, const void* data, size_t size = 0) const;
	ComPtr<ID3D11ShaderResourceView> CreateShaderResource(DXGI_FORMAT format, int width, int height, const void* pData) const;

	inline IDXGIFactory7* GetFactory() const { return m_factory.Get(); }
	inline ID3D11Device5* GetDevice() const { return m_device.Get(); }
	inline ID3D11DeviceContext4* GetContext() const { return m_context.Get(); }
private:
	Graphics();

	~Graphics();

	ComPtr<IDXGIFactory7> m_factory;
	ComPtr<ID3D11Device5> m_device;
	ComPtr<ID3D11DeviceContext4> m_context;
	ComPtr<ID3D11RenderTargetView1> m_rtv;
	ComPtr<IDXGIAdapter4> m_adapter;
	ComPtr<ID3D11InputLayout> m_inputLayout;
	ComPtr<ID3D11PixelShader> m_pixelShader;
	ComPtr<ID3D11VertexShader> m_vertexShader;
	ComPtr<ID3D11DepthStencilView> m_depthStencilView;

#ifdef _DEBUG
	ComPtr<ID3D11Debug> m_debug;
#endif

	ComPtr<ID3D11SamplerState> m_sampler;

	unique_ptr<SwapChain> m_swapChain;
	ComPtr<ID3D11Buffer> m_CBVSFrame;
	ComPtr<ID3D11Buffer> m_CBVSModel;
	ComPtr<ID3D11Buffer> m_CBVSWindow;
	ComPtr<ID3D11Buffer> m_CBPSFrame;
	ComPtr<ID3D11Buffer> m_vertexBuffer;
	ComPtr<ID3D11Buffer> m_indexBuffer;

	D3D_FEATURE_LEVEL m_featureLevel;

	mutex m_render_mutex;
};

