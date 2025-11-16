#include "PixelShaderBuilder.h"

#include <d3dcompiler.h>
#include <winerror.h>
#include <winnt.h>

#include "APIResources.h"
#include "LoggerFactory.h"
#include "PixelShader.h"

PixelShaderBuilder::PixelShaderBuilder(const std::string& filename,
                                       const std::string& entrypoint)
    : filename(filename), entrypoint(entrypoint) {}

Result<PixelShader, ShaderError> PixelShaderBuilder::create() {
    Logger logger = LoggerFactory::getLogger("Shader builder");

    std::wstring w_filename = {filename.begin(), filename.end()};

    Microsoft::WRL::ComPtr<ID3DBlob> bytecode, error;
    HRESULT result = D3DCompileFromFile(
        w_filename.c_str(), nullptr, nullptr, entrypoint.c_str(), "ps_5_0",
        D3DCOMPILE_ENABLE_STRICTNESS, NULL, bytecode.GetAddressOf(),
        error.GetAddressOf());

    if (FAILED(result)) {
        logger.warning("Failed to compile pixel shader {}: {}", filename,
                       (char*)error->GetBufferPointer());
        return ShaderError::CompileError;
    }

    logger.debug("Compiled pixel shader {}", filename);
    auto device = APIResources::get().getDevice();

    Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;
    device->CreatePixelShader(bytecode->GetBufferPointer(),
                              bytecode->GetBufferSize(), nullptr,
                              shader.GetAddressOf());
    return PixelShader(shader);
}