#include "MeshRegistry.h"

#include "MeshReader.h"

Mesh MeshRegistry::get(const std::string& name) {
    auto model = models.find(name);

    if (model == models.end()) return load(name);

    return model->second;
}

Mesh MeshRegistry::load(const std::string& name) {
    Mesh loaded = Assets::MeshReader().read(name);

    models.emplace(name, loaded);

    return loaded;
}
