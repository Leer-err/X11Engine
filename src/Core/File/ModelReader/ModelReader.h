#pragma once

#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <cstdint>
#include <vector>

#include "Mesh.h"
#include "VertexFormats.h"

namespace File {

class ModelReader {
   public:
    explicit ModelReader(const std::string& filename);

    Mesh readMesh() const;

   private:
    std::vector<Vertex> readVertices() const;
    std::vector<uint32_t> readIndices() const;

    Assimp::Importer importer;

    aiMesh* mesh;
};

}  // namespace File