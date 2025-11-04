#ifndef BASIC_PIPELINE_SYSTEMS_H
#define BASIC_PIPELINE_SYSTEMS_H

#include "ISystem.h"

class World;

class PreUpdate : public ISystem {
   public:
    bool prepare(World& world) override { return true; }
    void update(World& world, float delta_time) override {}
};

class PreSimulation : public ISystem {
   public:
    bool prepare(World& world) override { return true; }
    void update(World& world, float delta_time) override {}
};

class Simulation : public ISystem {
   public:
    bool prepare(World& world) override { return true; }
    void update(World& world, float delta_time) override {}
};

class PostSimulation : public ISystem {
   public:
    bool prepare(World& world) override { return true; }
    void update(World& world, float delta_time) override {}
};

class Update : public ISystem {
   public:
    bool prepare(World& world) override { return true; }
    void update(World& world, float delta_time) override {}
};

class PostUpdate : public ISystem {
   public:
    bool prepare(World& world) override { return true; }
    void update(World& world, float delta_time) override {}
};

class Rendering : public ISystem {
   public:
    bool prepare(World& world) override { return true; }
    void update(World& world, float delta_time) override {}
};

#endif  // BASIC_PIPELINE_SYSTEMS_H