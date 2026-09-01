#include "MeshRegistry.h"

#include <optional>

namespace Graphics {

MeshRegistry::MeshRegistry() : next_handle(0) {}

MeshHandle MeshRegistry::addMesh(const Mesh& mesh) {
    meshes.push_back(mesh);
    auto handle = next_handle;
    next_handle++;

    return handle;
}

std::optional<Mesh> MeshRegistry::getMesh(MeshHandle handle) const {
    if (handle >= meshes.size()) {
        return {};
    }

    return meshes.at(handle);
}

MeshHandle MeshRegistry::addMesh(const std::string& name, const Mesh& mesh) {
    auto handle = addMesh(mesh);

    mesh_names.try_emplace(name, handle);

    return handle;
}

std::optional<Mesh> MeshRegistry::getMesh(std::string_view name) const {
    auto it = mesh_names.find(name);
    if (it == mesh_names.end()) return {};

    return getMesh(it->second);
}

}  // namespace Graphics