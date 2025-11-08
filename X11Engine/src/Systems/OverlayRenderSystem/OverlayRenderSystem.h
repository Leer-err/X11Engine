#ifndef OVERLAY_RENDER_SYSTEM_H
#define OVERLAY_RENDER_SYSTEM_H

#include <memory>
#include <vector>

#include "Entity.h"
#include "IResourceFactory.h"
#include "ISystem.h"
#include "World.h"

class OverlayRenderSystem : public ISystem {
   public:
    OverlayRenderSystem(std::shared_ptr<IResourceFactory> resource_factory);
    ~OverlayRenderSystem();

    bool prepare(World& world) override;
    void update(World& world, float delta_time) override;

   private:
    void drawHeader(float time);
    void drawWorldInfo(World& world);

    void drawEntityInfo(const Entity& entity);
    void drawPositionInfo(const Entity& entity);
    void drawPhysicsInfo(const Entity& entity);
    void drawChildrenInfo(const Entity& entity);

    std::vector<float> frame_times;
    int max_frame_times;

    bool is_showing;
};

#endif  // OVERLAY_RENDER_SYSTEM_H