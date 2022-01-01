#pragma once
#include <wrl/client.h>
#include <d3d11_4.h>
using Microsoft::WRL::ComPtr;

class Buffer
{
public:
	Buffer(const D3D11_USAGE usage, const D3D11_BIND_FLAG bind, ID3D11Device5* device, const void* data = nullptr, const size_t dataSize = 0);

	ID3D11Buffer* Get();
	ID3D11Buffer** GetAddress();
private:
	ComPtr<ID3D11Buffer> m_buffer;
};

