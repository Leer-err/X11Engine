#pragma once

#include <string>

#include "Result.h"
#include "ShaderError.h"
#include "VertexShader.h"

class VertexShaderBuilder {
   public:
    VertexShaderBuilder(const std::string& filename,
                        const std::string& entrypoint = "main");

    Result<VertexShader, ShaderError> create();

   private:
    std::string filename;
    std::string entrypoint;
};