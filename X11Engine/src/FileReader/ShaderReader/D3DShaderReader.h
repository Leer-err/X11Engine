#ifndef D3D_SHADER_READER
#define D3D_SHADER_READER

#include <memory>
#include <string>

#include "IShaderBlob.h"
#include "IShaderReader.h"


class D3DShaderReader : public IShaderReader {
   public:
    std::shared_ptr<IShaderBlob> readPixelShader(
        const std::string& filename) override;
    std::shared_ptr<IShaderBlob> readVertexShader(
        const std::string& filename) override;
};
#endif  // D3D_SHADER_READER