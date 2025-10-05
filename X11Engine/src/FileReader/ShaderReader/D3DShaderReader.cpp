#include "D3DShaderReader.h"

#include <d3dcompiler.h>
#include <winerror.h>
#include <winnt.h>
#include <wrl/client.h>

#include <cstddef>
#include <memory>
#include <string>

#include "D3DShaderBlob.h"
#include "IShaderBlob.h"
#include "LoggerFactory.h"

std::shared_ptr<IShaderBlob> D3DShaderReader::readPixelShader(
    const std::string& filename) {
    Microsoft::WRL::ComPtr<ID3DBlob> shader, error;

    Logger logger = LoggerFactory::getLogger("D3D Shader reader");
    logger.debug("Compiling pixel shader {}", filename);

    std::wstring w_filename = {filename.begin(), filename.end()};

    HRESULT result =
        D3DCompileFromFile(w_filename.c_str(), nullptr, nullptr, "main",
                           "ps_5_0", D3DCOMPILE_ENABLE_STRICTNESS, NULL,
                           shader.GetAddressOf(), error.GetAddressOf());

    if (FAILED(result)) {
        logger.warning("Failed to compile pixel shader {}: {}", filename,
                       (char*)error->GetBufferPointer());
        return nullptr;
    }

    logger.info("Compiled pixel shader {}", filename);

    return std::make_shared<D3DShaderBlob>(shader);
}

std::shared_ptr<IShaderBlob> D3DShaderReader::readVertexShader(
    const std::string& filename) {
    Microsoft::WRL::ComPtr<ID3DBlob> shader, error;

    Logger logger = LoggerFactory::getLogger("D3D Shader reader");
    logger.debug("Compiling vertex shader {}", filename);

    std::wstring w_filename = {filename.begin(), filename.end()};

    HRESULT result =
        D3DCompileFromFile(w_filename.c_str(), nullptr, nullptr, "main",
                           "vs_5_0", D3DCOMPILE_ENABLE_STRICTNESS, NULL,
                           shader.GetAddressOf(), error.GetAddressOf());

    if (FAILED(result)) {
        logger.warning("Failed to compile vertex shader {}: {}", filename,
                       (char*)error->GetBufferPointer());
        return nullptr;
    }

    logger.info("Compiled vertex shader {}", filename);

    return std::make_shared<D3DShaderBlob>(shader);
}