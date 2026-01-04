#ifndef BINDING_H
#define BINDING_H

#include <cstdint>

typedef uint8_t RegisterIndex;

struct Binding {
    enum class ShaderType { Vertex, Pixel };

    constexpr Binding() noexcept {}
    constexpr Binding(const ShaderType shader_type,
                      const RegisterIndex index) noexcept
        : shader_type(shader_type), index(index) {}

    ShaderType shader_type;
    RegisterIndex index;
};

#endif  // BINDING_H