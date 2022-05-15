#pragma once
#include <d3d11_4.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <memory>
#include <mutex>
#include "Helper.h"
#include <vector>
#include "Vector3.h"

#include "Components/CameraComponent.h"

using Microsoft::WRL::ComPtr;
using std::unique_ptr;
using std::vector;
using std::mutex;

class SwapChain;
class Buffer;

class Graphics
{
public:
	Graphics(UINT width, UINT height, HWND hWnd);

	~Graphics();

	void PreFrame(CameraComponent* camera);
	void PostFrame();
	void Draw(const vector<vector3>& vertices, const vector<uint32_t>& indices);
	inline IDXGIFactory7* GetFactory() const { return m_factory.Get(); }
	inline ID3D11Device5* GetDevice() const { return m_device.Get(); }
private:
	ComPtr<IDXGIFactory7> m_factory;
	ComPtr<ID3D11Device5> m_device;
	ComPtr<ID3D11DeviceContext4> m_context;
	ComPtr<ID3D11RenderTargetView1> m_rtv;
	ComPtr<IDXGIAdapter4> m_adapter;
	ComPtr<ID3D11InputLayout> m_inputLayout;
	ComPtr<ID3D11PixelShader> m_pixelShader;
	ComPtr<ID3D11VertexShader> m_vertexShader;

	unique_ptr<SwapChain> m_swapChain;

	D3D_FEATURE_LEVEL m_featureLevel;

	mutex m_render_mutex;
};

