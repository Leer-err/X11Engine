#pragma once

#include "EngineData.h"
#include "FrameData.h"
#include "FrameGraph.h"

namespace Graphics {

class OverlayRenderer {
   public:
    OverlayRenderer();

    void render(const FrameData& frame_data);

   private:
};

}  // namespace Graphics