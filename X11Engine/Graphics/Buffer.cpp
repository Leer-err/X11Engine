#include "Buffer.h"
#include <d3d11_4.h>

Buffer::Buffer(const D3D11_USAGE usage, const D3D11_BIND_FLAG bind, ID3D11Device5* device, const void* data, const size_t dataSize)
{
	D3D11_BUFFER_DESC desc = {};
	desc.Usage = usage;
	desc.ByteWidth = dataSize;
	desc.BindFlags = bind;
	desc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA res = {};
	res.pSysMem = data;

	device->CreateBuffer(&desc, &res, &m_buffer);
}

ID3D11Buffer* Buffer::Get()
{
	return m_buffer.Get();
}

ID3D11Buffer** Buffer::GetAddress()
{
	return m_buffer.GetAddressOf();
}
