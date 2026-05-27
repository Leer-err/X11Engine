#pragma once

#include "EngineData.h"
#include "FrameData.h"

namespace Graphics {

class PostProcessingPass {
   public:
    PostProcessingPass(EngineData engine_data);

    void render(const FrameData& frame_data);

   private:
};

}  // namespace Graphics