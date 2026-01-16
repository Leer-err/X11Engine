#ifndef I_SYSTEM_H
#define I_SYSTEM_H

class World;

class ISystem {
   public:
    virtual void update(World& world, float delta_time) {}

    virtual void preSimulate(World& world) {}
    virtual void simulate(World& world) {}
    virtual void preRender(World& world) {}
    virtual void render(World& world) {}
};

#endif  // I_SYSTEM_H