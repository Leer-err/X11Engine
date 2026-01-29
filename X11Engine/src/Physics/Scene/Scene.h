#pragma once

namespace Physics {

class Scene {
   public:
    Scene();

    void addActor(const Actor& actor);
    void simulate(float delta_time);

   private:
    physx::PxScene* scene;
};

};  // namespace Physics