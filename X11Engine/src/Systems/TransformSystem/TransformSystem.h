#ifndef TRANSFORM_SYSTEM_H
#define TRANSFORM_SYSTEM_H

#include "ISystem.h"

class Entity;

class TransformSystem : public ISystem {
   public:
    void preSimulate(World& world) override;
    void preRender(World& world) override;

   private:
    void updateTransforms(World& world);
    void updateSingleTransform(Entity entity);

    void markChildrenDirty(Entity parent);
};

#endif  // TRANSFORM_SYSTEM_H