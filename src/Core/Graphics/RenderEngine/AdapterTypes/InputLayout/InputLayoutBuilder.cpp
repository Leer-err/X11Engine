#include "InputLayoutBuilder.h"

#include <spirv_reflect.h>
#include <vulkan/vulkan.h>

#include <cstddef>
#include <cstdint>
#include <vector>

#include "InputLayout.h"
#include "ShaderRegistry.h"

namespace Graphics {

constexpr size_t getFormatSize(VkFormat format) {
    switch (format) {
        case VK_FORMAT_R32G32B32_SFLOAT:
            return sizeof(float) * 3;
        case VK_FORMAT_R32G32_SFLOAT:
            return sizeof(float) * 2;
        default:
            return 0;
    }

    return 0;
}

InputLayoutBuilder::InputLayoutBuilder(ShaderRegistry& shader_registry,
                                       const std::string& vertex_shader_file)
    : shader_registry(shader_registry),
      vertex_shader_file(vertex_shader_file) {}

InputLayoutBuilder::~InputLayoutBuilder() {}

Result<InputLayout, InputLayoutBuilder::Error> InputLayoutBuilder::create() {
    if (vertex_shader_file.empty() == false) {
        auto shader_push_constants =
            getPushConstantsFromShader(vertex_shader_file);

        if (!shader_push_constants.isError()) {
            push_constant_ranges = shader_push_constants.getResult();
        }
    }

    std::vector<VkVertexInputAttributeDescription> vertexAttributes;

    size_t vertex_size = 0;
    size_t element_index = 0;

    auto result = InputLayout{};
    result.push_constants = push_constant_ranges;

    return result;
}

Result<VkFormat, InputLayoutBuilder::Error> InputLayoutBuilder::parseType(
    const SpvReflectFormat& type) {
    switch (type) {
        case SPV_REFLECT_FORMAT_R32_SFLOAT:
            return VK_FORMAT_R32_SFLOAT;
        case SPV_REFLECT_FORMAT_R32G32_SFLOAT:
            return VK_FORMAT_R32G32_SFLOAT;
        case SPV_REFLECT_FORMAT_R32G32B32_SFLOAT:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case SPV_REFLECT_FORMAT_R32_SINT:
            return VK_FORMAT_R32_SINT;
        case SPV_REFLECT_FORMAT_R32G32_SINT:
            return VK_FORMAT_R32G32_SINT;
        case SPV_REFLECT_FORMAT_R32G32B32_SINT:
            return VK_FORMAT_R32G32B32_SINT;
        case SPV_REFLECT_FORMAT_R32G32B32A32_SINT:
            return VK_FORMAT_R32G32B32A32_SINT;
        case SPV_REFLECT_FORMAT_R32_UINT:
            return VK_FORMAT_R32_UINT;
        case SPV_REFLECT_FORMAT_R32G32_UINT:
            return VK_FORMAT_R32G32_UINT;
        case SPV_REFLECT_FORMAT_R32G32B32_UINT:
            return VK_FORMAT_R32G32B32_UINT;
        case SPV_REFLECT_FORMAT_R32G32B32A32_UINT:
            return VK_FORMAT_R32G32B32A32_UINT;
        default:
            return Error::UnsupportedElementFormat;
    }
}

Result<std::vector<size_t>, InputLayoutBuilder::Error>
InputLayoutBuilder::getPushConstantsFromShader(
    const std::string& vertex_shader_file) {
    auto shader_source = shader_registry.getShaderBytecode(vertex_shader_file);
    if (!shader_source) return Error::FileNotFound;

    SpvReflectShaderModule module;
    SpvReflectResult result = spvReflectCreateShaderModule(
        shader_source->size(), shader_source->data(), &module);
    if (result != SPV_REFLECT_RESULT_SUCCESS) return Error::ParseError;

    uint32_t var_count = 0;
    result = spvReflectEnumeratePushConstantBlocks(&module, &var_count, NULL);
    if (result != SPV_REFLECT_RESULT_SUCCESS) return Error::ParseError;

    auto push_vars = new SpvReflectBlockVariable*[var_count];
    result =
        spvReflectEnumeratePushConstantBlocks(&module, &var_count, push_vars);
    if (result != SPV_REFLECT_RESULT_SUCCESS) {
        delete[] push_vars;
        return Error::ParseError;
    }

    auto push_constants = std::vector<size_t>(var_count);
    for (int i = 0; i < var_count; i++) {
        push_constants[i] = push_vars[i]->size;
    }

    delete[] push_vars;

    return push_constants;
}

}  // namespace Graphics