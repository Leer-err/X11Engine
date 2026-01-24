#include "TextureBindings.h"

#include <lua.hpp>
#include <string>
#include <tracy/Tracy.hpp>

#include "ImageReaderFactory.h"
#include "LuaUtility.h"
#include "TextureBuilder.h"

namespace Utility = Engine::Script::Utility;

constexpr char TEXTURE_METATABLE[] = "Texture";

extern "C" int fromFile(lua_State* state) {
    ZoneScoped;

    std::string filename = Utility::getArgument<std::string>(state, 2);

    auto reader = ImageReaderFactory::get().getReaderByFileExtension(filename);
    auto image = reader->readFromFile(filename);
    auto texture_result =
        TextureBuilder::fromImage(image).isShaderResource().create();

    if (texture_result.isError()) return Utility::pushNil(state);
    auto texture = texture_result.getResult();

    Utility::pushUserData(state, TEXTURE_METATABLE, std::move(texture));

    return 1;
}

static void createTextureMetatable(lua_State* state) {
    lua_createtable(state, 0, 5);

    lua_pushvalue(state, -1);
    Utility::setMemberName(state, "__index");
    Utility::setMemberFunction(state, "fromFile", fromFile);
    Utility::setGlobalName(state, TEXTURE_METATABLE);
}

namespace Engine::Script::Binding {

Texture getTexture(lua_State* state, int index) {
    auto texture = Utility::getUserData<Texture>(state, index);
    return *texture;
}

void initTextureBindings(lua_State* state) { createTextureMetatable(state); }

};  // namespace Engine::Script::Binding