#include "ShaderRegistry.h"

#include <stdexcept>

#include "PixelShader.h"
#include "PixelShaderBuilder.h"
#include "VertexShaderBuilder.h"

PixelShader ShaderRegistry::getPixelShader(const std::string& name) {
    auto it = pixel_shaders.find(name);
    if (it != pixel_shaders.end()) return it->second;

    auto result = PixelShaderBuilder(name, "main").create();
    if (result.isError()) throw std::runtime_error("Shader compilation error");

    auto shader = result.getResult();
    pixel_shaders.emplace(name, shader);

    return shader;
}

VertexShader ShaderRegistry::getVertexShader(const std::string& name) {
    auto it = vertex_shaders.find(name);
    if (it != vertex_shaders.end()) return it->second;

    auto result = VertexShaderBuilder(name, "main").create();
    if (result.isError()) throw std::runtime_error("Shader compilation error");

    auto shader = result.getResult();
    vertex_shaders.emplace(name, shader);

    return shader;
}