#ifndef MODEL_H
#define MODEL_H

#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "IIndexBuffer.h"
#include "IPixelShader.h"
#include "ITexture.h"
#include "IVertexBuffer.h"
#include "IVertexShader.h"
#include "Matrix.h"

struct BoneReference {
    std::string bone_name;
    size_t skeleton_node_index;
    Matrix offset_matrix;
};

struct Mesh {
    std::shared_ptr<IIndexBuffer> index_buffer;
    std::shared_ptr<IVertexBuffer> vertex_buffer;

    std::vector<BoneReference> bones;
};

struct Material {
    std::shared_ptr<ITexture> albedo_texture;
    std::shared_ptr<ITexture> roughness_texture;
    std::shared_ptr<ITexture> normal_texture;
};

struct Skeleton {
    std::vector<std::string> bone_names;
    std::vector<size_t> parents;
    std::vector<Matrix> transforms;
    std::vector<Matrix> inverse_transforms;
    std::unordered_map<std::string, size_t> bone_indices;
};

struct CurrentAnimation {
    float current_time;
    std::string animation_name;
};

struct Model {
    std::string name;
    std::vector<Mesh> meshes;
    Material material;
    Skeleton skeleton;
};

#endif  // MODEL_H
