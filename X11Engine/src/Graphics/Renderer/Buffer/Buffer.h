#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <cstddef>

class Buffer {
    friend class BufferBuilder;
    friend class Context;

   public:
    Buffer() {}

    size_t getSize() const;
    size_t getOffset() const;
    size_t getStride() const;

   protected:
    Buffer(Microsoft::WRL::ComPtr<ID3D11Buffer> buffer, size_t size,
           size_t stride = 0, size_t offset = 0);

    Microsoft::WRL::ComPtr<ID3D11Buffer> get() const;

   private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;

    size_t size;
    size_t offset;
    size_t stride;
};