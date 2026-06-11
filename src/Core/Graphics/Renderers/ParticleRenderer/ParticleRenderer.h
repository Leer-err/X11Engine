#pragma once

#include "FrameData.h"

namespace Graphics {

class ParticleRenderer {
   public:
    void render(const FrameData& frame_data, const VFXWorld& world);

   private:
};

}  // namespace Graphics