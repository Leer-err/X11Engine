#include "EssentialsLoader.h"

#include <filesystem>

#include "Graphics.h"
#include "IRenderEngine.h"
#include "LoggerFactory.h"
#include "ModelReader.h"

namespace fs = std::filesystem;

namespace Essentials {

constexpr auto TEXTURES_LIST = {""};
constexpr auto MESHES_LIST = {"Sphere.fbx", "SkySphere.fbx", "Quad.fbx",
                              "ScreenQuad.fbx"};

static bool readMesh(const fs::path& path,
                     Graphics::IRenderEngine* render_engine) {
    File::ModelReader reader(path.string());

    auto mesh = reader.readMesh();

    render_engine->addMesh(path.stem().string(), mesh);

    return true;
}

bool load() {
    auto logger = LoggerFactory::getLogger("EssentialsLoader");
    auto render_engine = Graphics::getRenderEngine();

    auto base = fs::path("./Assets/");

    auto result = true;

    for (const auto& texture : TEXTURES_LIST) {
        auto path = base / texture;
    }

    for (const auto& mesh : MESHES_LIST) {
        auto path = base / mesh;

        if (readMesh(path, render_engine) == false) {
            result = false;
            logger.error("Failed to load mesh: {}", mesh);
        }
    }

    return result;
}

}  // namespace Essentials