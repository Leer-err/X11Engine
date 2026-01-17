#pragma once

#include <string>
#include <vector>

#include "Mesh.h"

struct aiMesh;

struct StaticVertex;

namespace Assets {

class MeshReader {
   public:
    MeshReader();

    Mesh readStatic(const std::string& filename) const;

   private:
    std::vector<uint32_t> readIndices(const aiMesh* mesh) const;

    std::vector<StaticVertex> readStaticVertices(const aiMesh* mesh) const;
};

};  // namespace Assets