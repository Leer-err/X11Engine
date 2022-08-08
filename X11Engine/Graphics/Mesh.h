#pragma once
#include <stdint.h>
#include <wrl/client.h>

#include <vector>

#include "Types/Vertex.h"

using Microsoft::WRL::ComPtr;
using std::vector;

struct ID3D11Buffer;

struct Mesh {
    Mesh(ComPtr<ID3D11Buffer> vertices, ComPtr<ID3D11Buffer> indices,
         uint32_t materialIndex)
        : vertices(vertices), indices(indices), materialIndex(materialIndex) {}

    ComPtr<ID3D11Buffer> vertices;
    ComPtr<ID3D11Buffer> indices;
    uint32_t materialIndex;
};