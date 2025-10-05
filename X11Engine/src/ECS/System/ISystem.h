#ifndef I_SYSTEM_H
#define I_SYSTEM_H

class World;

class ISystem {
   public:
    virtual bool prepare() = 0;

    virtual void update(World& world, float delta_time) = 0;
};

#endif  // I_SYSTEM_H