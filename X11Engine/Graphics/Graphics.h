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
public:
	static Graphics& get() {
		static Graphics instance;
		return instance;
	}

	void PreFrame();
	void PostFrame();
	void Draw(const Model& model, const matrix& mvpMatrix);

	ComPtr<ID3D11Buffer> CreateBuffer(const D3D11_USAGE usage, const D3D11_BIND_FLAG bind, const void* data, const size_t dataSize) const;
	void UpdateBuffer(const ComPtr<ID3D11Buffer>& buf, const void* data, size_t size) const;
	ComPtr<ID3D11Texture2D> CreateTexture(int width, int height, const void* pData) const;

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

	ComPtr<ID3D11SamplerState> m_sampler;

	unique_ptr<SwapChain> m_swapChain;
	ComPtr<ID3D11Buffer> m_cbFrame;
	ComPtr<ID3D11Buffer> m_cbModel;
	ComPtr<ID3D11Buffer> m_cbDraw;
	ComPtr<ID3D11Buffer> m_vertexBuffer;
	ComPtr<ID3D11Buffer> m_indexBuffer;

	D3D_FEATURE_LEVEL m_featureLevel;

	mutex m_render_mutex;
};

