#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "GraphicsMesh.h"
#include "MeshHandle.h"
#include "TransparentStringHash.h"

namespace Graphics {

class MeshRegistry {
   public:
    MeshRegistry();

    MeshHandle addMesh(const Mesh& mesh);
    MeshHandle addMesh(const std::string& name, const Mesh& mesh);
    std::optional<Mesh> getMesh(MeshHandle handle) const;
    std::optional<Mesh> getMesh(std::string_view name) const;

   private:
    MeshHandle next_handle;
    std::vector<Mesh> meshes;

    std::unordered_map<std::string, MeshHandle, TransparentStringHash,
                       std::equal_to<>>
        mesh_names;
};

}  // namespace Graphics