#include "ShaderBuilder.h"

#include <vulkan/vulkan.h>

#include <optional>

#include "Shader.h"
#include "ShaderError.h"
#include "ShaderRegistry.h"

namespace Graphics {

ShaderBuilder::ShaderBuilder(const std::string& filename,
                             const std::string& entrypoint,
                             VkShaderStageFlagBits stage)
    : filename(filename), entrypoint(entrypoint), stage(stage) {}

Result<Shader, ShaderError> ShaderBuilder::create(
    ShaderRegistry& shader_registry) const {
    auto shader_module = shader_registry.getModule(filename);
    if (shader_module == std::nullopt) return ShaderError::NotFound;

    auto result = Shader{};
    result.shader = *shader_module;
    result.stage = stage;
    result.entrypoint = entrypoint;
    result.filename = filename;

    return result;
}

}  // namespace Graphics