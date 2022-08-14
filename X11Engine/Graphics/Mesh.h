#pragma once
#include <stdint.h>
#include <wrl/client.h>

#include <vector>

#include "Types/Vertex.h"

using Microsoft::WRL::ComPtr;
using std::vector;

struct ID3D11Buffer;

struct IndexBuffer {
    IndexBuffer() = default;
    IndexBuffer(const IndexBuffer&) = default;
    IndexBuffer(IndexBuffer&&) = default;
    IndexBuffer& operator=(const IndexBuffer&) = default;
    IndexBuffer& operator=(IndexBuffer&&) = default;

    IndexBuffer(const ComPtr<ID3D11Buffer>& buffer, uint32_t indexCount)
        : buffer(buffer), indexCount(indexCount) {}
    ComPtr<ID3D11Buffer> buffer;
    uint32_t indexCount;
};

struct Mesh {
    Mesh(const ComPtr<ID3D11Buffer>& vertices, const IndexBuffer& indices,
         uint32_t materialIndex)
        : vertices(vertices), indices(indices), materialIndex(materialIndex) {}

    ComPtr<ID3D11Buffer> vertices;
    IndexBuffer indices;
    uint32_t materialIndex;
};