#pragma once

#include "Binding.h"
#include "Matrix.h"
#include "Texture.h"

namespace Asset {
namespace StaticModel {

namespace Data {

static constexpr Binding camera_parameters =
    Binding(Binding::ShaderType::Vertex, 0);
static constexpr Binding world_matrix = Binding(Binding::ShaderType::Vertex, 1);

static constexpr Binding albedo_texture =
    Binding(Binding::ShaderType::Pixel, 0);
static constexpr Binding albedo_sampler =
    Binding(Binding::ShaderType::Pixel, 0);

struct WorldMatrixBuffer {
    Matrix world_matrix;
};

};  // namespace Data

struct Material {
    Texture albedo_texture;
};

};  // namespace StaticModel
};  // namespace Asset