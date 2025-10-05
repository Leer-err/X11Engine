#ifndef PLAYER_INPUT_SYSTEM
#define PLAYER_INPUT_SYSTEM

#include "ISystem.h"
#include "World.h"

class PlayerMovementSystem : public ISystem {
   public:
    bool prepare() override;

    void update(World& world, float delta_time) override;

   private:
};

#endif  // PLAYER_INPUT_SYSTEM