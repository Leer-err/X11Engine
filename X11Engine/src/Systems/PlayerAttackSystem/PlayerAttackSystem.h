#ifndef PLAYER_ATTACK_SYSTEM
#define PLAYER_ATTACK_SYSTEM

#include "ISystem.h"

class PlayerAttackSystem : public ISystem {
   public:
    bool prepare(World& world) override;

    void update(World& world, float delta_time) override;

   private:
};

#endif  // PLAYER_ATTACK_SYSTEM