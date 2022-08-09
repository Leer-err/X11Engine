#pragma once
#include <stdint.h>
#include <wrl/client.h>

#include <vector>

#include "Types/Vertex.h"

using Microsoft::WRL::ComPtr;
using std::vector;

struct ID3D11Buffer;

struct IndexBuffer {
    IndexBuffer(const ComPtr<ID3D11Buffer>& buffer, uint32_t indexCount)
        : buffer(buffer), indexCount(indexCount) {}
    ComPtr<ID3D11Buffer> buffer;
    uint32_t indexCount;
};

struct Mesh {
    Mesh(const ComPtr<ID3D11Buffer>& vertices,
         const ComPtr<ID3D11Buffer>& indices, uint32_t indexCount,
         uint32_t materialIndex)
        : vertices(vertices),
          indices(indices, indexCount),
          materialIndex(materialIndex) {}

    ComPtr<ID3D11Buffer> vertices;
    IndexBuffer indices;
    uint32_t materialIndex;
};