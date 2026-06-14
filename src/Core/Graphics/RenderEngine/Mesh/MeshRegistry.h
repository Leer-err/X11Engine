#pragma once

#include <optional>
#include <vector>

#include "GraphicsMesh.h"
#include "MeshHandle.h"

namespace Graphics {

class MeshRegistry {
   public:
    MeshRegistry();

    MeshHandle addMesh(const Mesh& mesh);
    std::optional<Mesh> getMesh(MeshHandle handle);

   private:
    MeshHandle next_handle;
    std::vector<Mesh> meshes;
};

}  // namespace Graphics