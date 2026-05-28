#pragma once

#include <vulkan/vulkan.h>

#include <string>

#include "Result.h"
#include "Shader.h"
#include "ShaderError.h"
#include "ShaderRegistry.h"

namespace Graphics {

class ShaderBuilder {
   public:
    ShaderBuilder(const std::string& filename, const std::string& entrypoint,
                  VkShaderStageFlagBits stage);

    Result<Shader, ShaderError> create(ShaderRegistry& shader_registry) const;

   private:
    std::string filename;
    std::string entrypoint;
    VkShaderStageFlagBits stage;
};

}  // namespace Graphics