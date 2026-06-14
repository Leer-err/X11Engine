#pragma once

#include <spirv_reflect.h>
#include <vulkan/vulkan.h>

#include <optional>
#include <vector>

#include "InputLayout.h"
#include "Result.h"
#include "ShaderRegistry.h"

namespace Graphics {

class InputLayoutBuilder {
   public:
    enum class Error {
        FileNotFound,
        ParseError,
        UnsupportedElementFormat,
    };

    InputLayoutBuilder(ShaderRegistry& shader_registry,
                       const std::string& vertex_shader_file);

    InputLayoutBuilder();
    ~InputLayoutBuilder();

    InputLayoutBuilder& addElement(VkFormat format);
    InputLayoutBuilder& setPushConstantsSize(size_t size);

    Result<InputLayout, Error> create();

   private:
    Result<std::vector<size_t>, Error> getPushConstantsFromShader(
        const std::string& vertex_shader_file);

    Result<VkFormat, Error> parseType(const SpvReflectFormat& type);

    ShaderRegistry& shader_registry;

    std::string vertex_shader_file;

    std::vector<size_t> push_constant_ranges;
};

}  // namespace Graphics