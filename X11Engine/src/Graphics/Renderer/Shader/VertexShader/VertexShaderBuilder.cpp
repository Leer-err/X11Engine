#include "VertexShaderBuilder.h"

#include <d3dcompiler.h>
#include <winerror.h>
#include <winnt.h>

#include "APIResources.h"
#include "LoggerFactory.h"

VertexShaderBuilder::VertexShaderBuilder(const std::string& filename,
                                         const std::string& entrypoint)
    : filename(filename), entrypoint(entrypoint) {}

Result<VertexShader, ShaderError> VertexShaderBuilder::create() {
    Logger logger = LoggerFactory::getLogger("Shader builder");

    std::wstring w_filename = {filename.begin(), filename.end()};

    Microsoft::WRL::ComPtr<ID3DBlob> bytecode, error;
    HRESULT result = D3DCompileFromFile(
        w_filename.c_str(), nullptr, nullptr, entrypoint.c_str(), "vs_5_0",
        D3DCOMPILE_ENABLE_STRICTNESS, NULL, bytecode.GetAddressOf(),
        error.GetAddressOf());

    if (FAILED(result)) {
        logger.warning("Failed to compile pixel shader {}: {}", filename,
                       (char*)error->GetBufferPointer());
        return ShaderError::CompileError;
    }

    logger.debug("Compiled pixel shader {}", filename);
    auto device = APIResources::get().getDevice();

    Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
    device->CreateVertexShader(bytecode->GetBufferPointer(),
                               bytecode->GetBufferSize(), nullptr,
                               shader.GetAddressOf());
    return VertexShader(shader, bytecode);
}