#ifndef SCRIPT_LOADER_H
#define SCRIPT_LOADER_H

#include <string_view>

#include "ScriptSandbox.h"

namespace Engine::Script {

class ScriptLoader {
   public:
    ScriptLoader(const ScriptSandbox& sandbox);

    void loadFromDirectory(std::string_view directory);

   private:
    ScriptSandbox sandbox;
};

};  // namespace Engine::Script

#endif  // SCRIPT_LOADER_H