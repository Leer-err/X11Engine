#include "ModelReader.h"

#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <string>
#include <string_view>

#include "Vector2.h"

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
    Mesh mesh;
    mesh.vertices = readVertices();
    mesh.indices = readIndices();

    return mesh;
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