#include "MeshReader.h"

#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <cstddef>
#include <vector>

#include "MeshBuilder.h"
#include "StaticVertex.h"
#include "Vector3.h"

namespace Assets {

MeshReader::MeshReader() {}

Mesh MeshReader::readStatic(const std::string& filename) const {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        filename.c_str(), aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                              aiProcess_JoinIdenticalVertices |
                              aiProcess_MakeLeftHanded | aiProcess_FlipUVs |
                              aiProcess_OptimizeGraph | aiProcess_SortByPType);

    if (scene == nullptr) throw;

    auto indices = readIndices(scene->mMeshes[0]);
    size_t indices_size = indices.size() * sizeof(uint32_t);

    auto vertices = readStaticVertices(scene->mMeshes[0]);
    size_t vertices_size = vertices.size() * sizeof(StaticVertex);

    return MeshBuilder()
        .addVertexData(vertices.data(), vertices_size, sizeof(StaticVertex))
        .setIndexData(indices.data(), indices_size)
        .create();
}

std::vector<uint32_t> MeshReader::readIndices(const aiMesh* mesh) const {
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

std::vector<StaticVertex> MeshReader::readStaticVertices(
    const aiMesh* mesh) const {
    std::vector<StaticVertex> vertices;

    size_t index_count = mesh->mNumVertices;
    vertices.reserve(index_count);

    for (int i = 0; i < mesh->mNumVertices; i++) {
        StaticVertex vertex;

        aiVector3D position = mesh->mVertices[i];
        vertex.position = Vector3(position.x, position.y, position.z) / 100;

        aiVector3D normal = mesh->mNormals[i];
        vertex.normal = Vector3(normal.x, normal.y, normal.z);

        aiVector3D tex_coords = mesh->mTextureCoords[0][i];
        vertex.uv.x = tex_coords.x;
        vertex.uv.y = tex_coords.y;

        vertices.push_back(vertex);
    }

    return vertices;
}

};  // namespace Assets