#include "ScriptLoader.h"

#include <filesystem>

#include "LoggerFactory.h"
#include "ScriptSandbox.h"

using namespace Engine::Script;
namespace fs = std::filesystem;

void ScriptLoader::loadFromDirectory(std::string_view directory) {
    auto path = fs::path(directory);

    if (!fs::is_directory(path))
        LoggerFactory::getLogger("ScriptLoader")
            .error("Directory {} not found", directory);

    for (const auto& entry : fs::directory_iterator(path)) {
        auto script_path = entry.path().string();

        ScriptSandbox::get().runFile(script_path);
    }
}