#pragma once

#include <map>
#include <string>
#include <unordered_map>

#include "PixelShader.h"
#include "VertexShader.h"

class ShaderRegistry {
   public:
    static ShaderRegistry& get() {
        static ShaderRegistry instance;
        return instance;
    }

    VertexShader getVertexShader(const std::string& name);
    PixelShader getPixelShader(const std::string& name);

    ShaderRegistry(const ShaderRegistry&) = delete;
    ShaderRegistry& operator=(const ShaderRegistry&) = delete;

    ShaderRegistry(ShaderRegistry&&) = delete;
    ShaderRegistry& operator=(ShaderRegistry&&) = delete;

   private:
    ShaderRegistry() {}

    std::unordered_map<std::string, VertexShader> vertex_shaders;
    std::unordered_map<std::string, PixelShader> pixel_shaders;
};