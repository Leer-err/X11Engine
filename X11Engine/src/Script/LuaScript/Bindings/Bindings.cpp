#include "Bindings.h"

#include "CameraBindings/CameraBindings.h"
#include "ECSBindings.h"
#include "InputBindings.h"

using namespace Engine::Script;

namespace Engine::Script::Binding {

void initBindings(lua_State* state) {
    Input::initInputBindings(state);
    ECS::initBindings(state);
    Camera::initBindings(state);
}

};  // namespace Engine::Script::Binding