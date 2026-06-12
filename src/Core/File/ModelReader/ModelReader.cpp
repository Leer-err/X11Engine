#include "ModelReader.h"

#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <string>
#include <string_view>

#include "Vector2.h"
#include "meshoptimizer.h"

namespace File {

ModelReader::ModelReader(const std::string& filename) : mesh(nullptr) {
    const aiScene* scene = importer.ReadFile(
        filename, aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                      aiProcess_JoinIdenticalVertices |
                      aiProcess_MakeLeftHanded | aiProcess_FlipUVs |
                      aiProcess_OptimizeGraph | aiProcess_SortByPType);

    if (scene == nullptr) return;

    mesh = scene->mMeshes[0];
}

Mesh ModelReader::readMesh() const {
    auto vertices = readVertices();
    auto indices = readIndices();

    const size_t max_vertices = 64;
    const size_t max_indices = 96;  // note: in v0.25 or prior, max_triangles
                                    // needs to be divisible by 4
    const float cone_weight = 0.0f;

    size_t max_meshlets =
        meshopt_buildMeshletsBound(indices.size(), max_vertices, max_indices);
    std::vector<meshopt_Meshlet> meshlets(max_meshlets);
    std::vector<unsigned int> meshlet_vertices(indices.size());
    std::vector<unsigned char> meshlet_triangles(indices.size());

    size_t meshlet_count = meshopt_buildMeshlets(
        meshlets.data(), meshlet_vertices.data(), meshlet_triangles.data(),
        indices.data(), indices.size(), (float*)&vertices[0].position,
        vertices.size(), sizeof(Vertex), max_vertices, max_indices,
        cone_weight);

    const auto& last = meshlets[meshlet_count - 1];

    meshlet_vertices.resize(last.vertex_offset + last.vertex_count);
    meshlet_triangles.resize(last.triangle_offset + last.triangle_count * 3);
    meshlets.resize(meshlet_count);

    for (const auto& meshlet : meshlets) {
        meshopt_optimizeMeshlet(&meshlet_vertices[meshlet.vertex_offset],
                                &meshlet_triangles[meshlet.triangle_offset],
                                meshlet.triangle_count, meshlet.vertex_count);
    }

    Mesh mesh;
    mesh.vertices = vertices;
    mesh.indices = indices;
    // mesh.meshlets =
}

std::vector<Vertex> ModelReader::readVertices() const {
    if (!mesh) return {};

    std::vector<Vertex> vertices;

    size_t index_count = mesh->mNumVertices;
    vertices.reserve(index_count);

    for (int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        aiVector3D position = mesh->mVertices[i];
        vertex.position = Vector3(position.x, position.y, position.z);

        aiVector3D tex_coords = mesh->mTextureCoords[0][i];
        vertex.uv = Vector2(tex_coords.x, tex_coords.y);

        vertices.push_back(vertex);
    }

    return vertices;
}

std::vector<uint32_t> ModelReader::readIndices() const {
    if (!mesh) return {};

    std::vector<uint32_t> indices;

    size_t index_count = mesh->mNumFaces * 3;
    indices.reserve(index_count);

    for (int i = 0; i < mesh->mNumFaces; i++) {
        const aiFace& face = mesh->mFaces[i];

        for (int index = 0; index < 3; index++) {
            int vertex_index = face.mIndices[index];
            indices.push_back(vertex_index);
        }
    }

    return indices;
}

}  // namespace File