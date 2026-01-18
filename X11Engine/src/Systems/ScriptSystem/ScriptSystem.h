#ifndef SCRIPT_SYSTEM_H
#define SCRIPT_SYSTEM_H

#include "ISystem.h"
// #include "ScriptSandbox.h"

class ScriptSystem : public ISystem {
   public:
    void update(World& world, float delta_time) override;

   private:
};

#endif  // SCRIPT_SYSTEM_H