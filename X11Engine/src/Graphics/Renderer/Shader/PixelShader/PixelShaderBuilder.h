#pragma once

#include <string>

#include "PixelShader.h"
#include "Result.h"
#include "ShaderError.h"

class PixelShaderBuilder {
   public:
    PixelShaderBuilder(const std::string& filename,
                       const std::string& entrypoint = "main");

    Result<PixelShader, ShaderError> create();

   private:
    std::string filename;
    std::string entrypoint;
};