#ifndef ASSIMP_MODEL_READER_H
#define ASSIMP_MODEL_READER_H

#include <memory>
#include <optional>
#include <string>

#include "IModelReader.h"
#include "IResourceFactory.h"
#include "IShaderReader.h"
#include "Logger.h"
#include "Model.h"
#include "assimp/anim.h"
#include "assimp/material.h"
#include "assimp/scene.h"

struct aiMesh;

class AssimpModelReader : public IModelReader {
   public:
    AssimpModelReader(std::shared_ptr<IResourceFactory> factory);
    std::optional<Model> read(const std::string& filename) override;

   private:
    Mesh parseMesh(const aiMesh* mesh, const Skeleton& skeleton);
    Skeleton parseSkeleton(aiNode* root_node);
    void parseSkeletonHelper(aiNode* node, Skeleton& skeleton,
                             const Matrix& transform);

    Material parseMaterial(const aiScene* scene, const aiMaterial* material);

    Logger logger;
    std::shared_ptr<IResourceFactory> factory;

    std::shared_ptr<IShaderReader> shader_reader;
};

#endif  // ASSIMP_MODEL_READER_H