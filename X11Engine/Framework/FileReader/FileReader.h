#pragma once

#include <d3d11_4.h>
#include <wrl/client.h>

#include <nlohmann/json.hpp>

using Microsoft::WRL::ComPtr;
using nlohmann::json;

namespace FileReader {
ComPtr<ID3D11Texture2D> ReadTexture2D(const char* filename);
json ReadJson(const char* filename);
}  // namespace FileReader