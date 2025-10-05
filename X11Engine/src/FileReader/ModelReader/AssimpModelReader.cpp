#include "AssimpModelReader.h"

#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <cstddef>
#include <cstring>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "ITexture.h"
#include "Image.h"
#include "ImageReaderFactory.h"
#include "Logger.h"
#include "LoggerFactory.h"
#include "Matrix.h"
#include "Model.h"
#include "Quaternion.h"
#include "Vector3.h"
#include "Vertex.h"
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/quaternion.h"
#include "assimp/texture.h"
#include "assimp/types.h"
#include "assimp/vector3.h"

AssimpModelReader::AssimpModelReader(std::shared_ptr<IResourceFactory> factory)
    : logger(LoggerFactory::getLogger("Assimp model reader")),
      factory(factory) {}

std::optional<Model> AssimpModelReader::read(const std::string& filename) {
    Assimp::Importer importer;

    logger.debug("Trying to read {}", filename);

    const aiScene* scene = importer.ReadFile(
        filename.c_str(), aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                              aiProcess_JoinIdenticalVertices |
                              aiProcess_MakeLeftHanded | aiProcess_FlipUVs |
                              aiProcess_OptimizeGraph | aiProcess_SortByPType);

    if (scene == nullptr) {
        logger.error("Failed to read {}", filename);
        return std::nullopt;
    }

    Skeleton skeleton = parseSkeleton(scene->mRootNode);

    std::vector<Mesh> meshes;
    for (int i = 0; i < scene->mNumMeshes; i++) {
        Mesh mesh = parseMesh(scene->mMeshes[i], skeleton);
        meshes.push_back(mesh);
    }

    Material material = parseMaterial(scene, scene->mMaterials[0]);

    return Model{scene->mName.C_Str(), meshes, material, skeleton};
}

Mesh AssimpModelReader::parseMesh(const aiMesh* mesh,
                                  const Skeleton& skeleton) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    for (int i = 0; i < mesh->mNumFaces; i++) {
        const aiFace& face = mesh->mFaces[i];

        for (int index = 0; index < face.mNumIndices; index++) {
            int vertex_index = face.mIndices[index];
            indices.push_back(vertex_index);
        }
    }

    for (int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        vertex.bone_ids[0] = -1;
        vertex.bone_ids[1] = -1;
        vertex.bone_ids[2] = -1;
        vertex.bone_ids[3] = -1;

        aiVector3D position = mesh->mVertices[i];

        vertex.position.x = position.x / 100;
        vertex.position.y = position.y / 100;
        vertex.position.z = position.z / 100;

        if (mesh->HasNormals()) {
            aiVector3D normal = mesh->mNormals[i];
            vertex.normal.x = normal.x;
            vertex.normal.y = normal.y;
            vertex.normal.z = normal.z;
        }

        aiVector3D tex_coords = mesh->mTextureCoords[0][i];
        vertex.uv.x = tex_coords.x;
        vertex.uv.y = tex_coords.y;

        vertices.push_back(vertex);
    }

    std::vector<BoneReference> bones;

    constexpr int MAX_BONE_INFLUENCE = 4;
    for (int i = 0; i < mesh->mNumBones; i++) {
        aiBone* bone = mesh->mBones[i];

        Matrix offset;
        memcpy(&offset, &bone->mOffsetMatrix, sizeof(Matrix));
        offset = offset.transpose();
        offset.matrix.m[3][0] /= 100.f;
        offset.matrix.m[3][1] /= 100.f;
        offset.matrix.m[3][2] /= 100.f;
        const char* name = bone->mName.C_Str();
        size_t index = skeleton.bone_indices.at(name);
        bones.push_back({std::string(name), index, offset});

        for (int j = 0; j < bone->mNumWeights; j++) {
            aiVertexWeight weight = bone->mWeights[j];

            int vertex_id = weight.mVertexId;

            Vertex& vertex = vertices[vertex_id];
            for (int k = 0; k < MAX_BONE_INFLUENCE; k++) {
                if (vertex.bone_ids[k] == -1) {
                    vertex.bone_ids[k] = i;
                    vertex.bone_weights[k] = weight.mWeight;
                    break;
                }
            }
        }
    }

    for (int i = 0; i < vertices.size(); i++) {
        Vertex& vertex = vertices[i];
        for (int j = 0; j < MAX_BONE_INFLUENCE; j++) {
            if (vertex.bone_ids[j] == -1) {
                vertex.bone_ids[j] = 0;
                vertex.bone_weights[j] = 0;
            }
        }
    }

    size_t vertex_buffer_size = vertices.size() * sizeof(Vertex);
    auto vertex_buffer = factory->createVertexBuffer((char*)vertices.data(),
                                                     vertex_buffer_size, false);

    size_t index_buffer_size = indices.size() * sizeof(uint32_t);
    auto index_buffer = factory->createIndexBuffer((char*)indices.data(),
                                                   index_buffer_size, false);

    return {index_buffer, vertex_buffer, bones};
}

Skeleton AssimpModelReader::parseSkeleton(aiNode* root_node) {
    Skeleton result;

    parseSkeletonHelper(root_node, result, Matrix::identity());

    return result;
}

void AssimpModelReader::parseSkeletonHelper(aiNode* node, Skeleton& skeleton,
                                            const Matrix& parent_transform) {
    aiVector3D assimp_position;
    aiQuaternion assimp_rotation;
    aiVector3D assimp_scaling;
    node->mTransformation.Decompose(assimp_scaling, assimp_rotation,
                                    assimp_position);
    assimp_position.x = assimp_position.x / 100;
    assimp_position.y = assimp_position.y / 100;
    assimp_position.z = assimp_position.z / 100;

    Vector3 position = {assimp_position.x, assimp_position.y,
                        assimp_position.z};
    Quaternion rotation = {assimp_rotation.w, assimp_rotation.x,
                           assimp_rotation.y, assimp_rotation.z};
    Vector3 scaling = {assimp_scaling.x, assimp_scaling.y, assimp_scaling.z};

    Matrix local_transform = Matrix::scale(scaling) *
                             Matrix::rotation(rotation) *
                             Matrix::translation(position);
    Matrix global_transform = local_transform * parent_transform;

    skeleton.transforms.push_back(local_transform);
    skeleton.inverse_transforms.push_back(global_transform.inverse());

    size_t bone_index = skeleton.bone_names.size();
    std::string name = node->mName.C_Str();

    skeleton.bone_indices.emplace(name, bone_index);
    skeleton.bone_names.push_back(name);

    if (node->mParent == nullptr) {
        skeleton.parents.push_back(bone_index);
    } else {
        std::string parent_name = node->mParent->mName.C_Str();
        auto parent_it = skeleton.bone_indices.find(parent_name);
        size_t parent_index = parent_it->second;
        skeleton.parents.push_back(parent_index);
    }

    for (int i = 0; i < node->mNumChildren; i++) {
        parseSkeletonHelper(node->mChildren[i], skeleton, global_transform);
    }
}

Material AssimpModelReader::parseMaterial(const aiScene* scene,
                                          const aiMaterial* assimp_material) {
    Material material = {};

    aiString path;
    auto result =
        assimp_material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), path);
    const aiTexture* assimp_texture = scene->GetEmbeddedTexture(path.C_Str());

    auto image_reader =
        ImageReaderFactory::get().getReaderByFileExtension(path.C_Str());

    Image texture_image = image_reader->readFromMemory(
        (const char*)assimp_texture->pcData, assimp_texture->mWidth);

    std::shared_ptr<ITexture> texture =
        factory->createTexture(texture_image, false, false);

    material.albedo_texture = texture;

    return material;
}