#ifndef SCRIPT_SYSTEM_H
#define SCRIPT_SYSTEM_H

#include "ISystem.h"
#include "ScriptSandbox.h"

class ScriptSystem : public ISystem {
   public:
    bool prepare(World& world) override;

    void update(World& world, float delta_time) override;

   private:
    Engine::Script::ScriptSandbox sandbox;
};

#endif  // SCRIPT_SYSTEM_H