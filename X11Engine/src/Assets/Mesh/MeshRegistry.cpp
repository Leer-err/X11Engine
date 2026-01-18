#include "MeshRegistry.h"

#include "LoggerFactory.h"
#include "MeshReader.h"

MeshRegistry::MeshRegistry()
    : logger(LoggerFactory::getLogger("MeshRegistry")) {}

Mesh MeshRegistry::get(const std::string& name) {
    auto model = meshes.find(name);

    if (model == meshes.end()) {
        logger.debug("{} is not found in cache, reading from file", name);
        return load(name);
    }

    logger.debug("Found {}", name);

    return model->second;
}

Mesh MeshRegistry::load(const std::string& name) {
    Mesh loaded = Assets::MeshReader().readStatic(name);

    meshes.emplace(name, loaded);

    return loaded;
}
