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

InputLayoutBuilder& InputLayoutBuilder::addElement(VkFormat format) {
    elements.push_back(format);

    return *this;
}

InputLayoutBuilder& InputLayoutBuilder::setPushConstantsSize(size_t size) {
    push_constants_size = size;

    return *this;
}

Result<InputLayout, InputLayoutBuilder::Error> InputLayoutBuilder::create() {
    if (vertex_shader_file.empty() == false) {
        // auto shader_elements = getElementsFromShader(vertex_shader_file);
        auto shader_push_constants =
            getPushConstantsFromShader(vertex_shader_file);

        // if (!shader_elements.isError() && !shader_push_constants.isError()) {
        //     elements = shader_elements.getResult();
        //     push_constants_size = shader_push_constants.getResult();
        // }
    }

    std::vector<VkVertexInputAttributeDescription> vertexAttributes;

    size_t vertex_size = 0;
    size_t element_index = 0;

    auto result = InputLayout{};
    result.elements.reserve(elements.size());

    for (const auto& format : elements) {
        auto format_size = getFormatSize(format);

        if (format_size == 0) return Error::UnsupportedElementFormat;

        VkVertexInputAttributeDescription desc = {};
        desc.binding = 0;
        desc.format = format;
        desc.location = element_index;
        desc.offset = vertex_size;

        result.elements.push_back(desc);

        vertex_size += format_size;
        element_index++;
    }

    VkVertexInputBindingDescription binding;
    binding.binding = 0;
    binding.stride = static_cast<uint32_t>(vertex_size);
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    result.buffer_binding_description = binding;
    result.push_constants_size = push_constants_size;

    return result;
}

Result<std::vector<VkFormat>, InputLayoutBuilder::Error>
InputLayoutBuilder::getElementsFromShader(
    const std::string& vertex_shader_file) {
    auto shader_source = shader_registry.getShaderBytecode(vertex_shader_file);
    if (!shader_source) return Error::FileNotFound;

    SpvReflectShaderModule module;
    SpvReflectResult result = spvReflectCreateShaderModule(
        shader_source->size(), shader_source->data(), &module);
    if (result != SPV_REFLECT_RESULT_SUCCESS) return Error::ParseError;

    uint32_t var_count = 0;
    result = spvReflectEnumerateInputVariables(&module, &var_count, NULL);
    if (result != SPV_REFLECT_RESULT_SUCCESS) return Error::ParseError;

    auto input_vars = new SpvReflectInterfaceVariable*[var_count];
    result = spvReflectEnumerateInputVariables(&module, &var_count, input_vars);
    if (result != SPV_REFLECT_RESULT_SUCCESS) {
        delete[] input_vars;
        return Error::ParseError;
    }

    auto inputs = std::vector<VkFormat>(var_count);
    for (int i = 0; i < var_count; i++) {
        auto element_type = parseType(input_vars[i]->format);
        if (element_type.isError()) {
            delete[] input_vars;
            return element_type.getError();
        }

        auto index = input_vars[i]->location;
        inputs[index] = element_type.getResult();
    }

    delete[] input_vars;

    return inputs;
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

Result<size_t, InputLayoutBuilder::Error>
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

    size_t size = 0;
    for (int i = 0; i < var_count; i++) {
        size += push_vars[i]->size;
    }

    delete[] push_vars;

    return size;
}

}  // namespace Graphics