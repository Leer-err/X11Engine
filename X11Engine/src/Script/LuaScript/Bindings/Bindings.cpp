#include "Bindings.h"

#include "ECSBindings.h"
#include "InputBindings.h"
#include "TextureBindings.h"

using namespace Engine::Script;

namespace Engine::Script::Binding {

void initBindings(lua_State* state) {
    Input::initInputBindings(state);
    ECS::initBindings(state);
    initTextureBindings(state);
}

};  // namespace Engine::Script::Binding