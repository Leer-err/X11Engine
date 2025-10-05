#ifndef I_SHADER_READER
#define I_SHADER_READER

#include <memory>
#include <string>

#include "IShaderBlob.h"

class IShaderReader {
   public:
    virtual std::shared_ptr<IShaderBlob> readPixelShader(
        const std::string& filename) = 0;
    virtual std::shared_ptr<IShaderBlob> readVertexShader(
        const std::string& filename) = 0;
};
#endif  // I_SHADER_READER