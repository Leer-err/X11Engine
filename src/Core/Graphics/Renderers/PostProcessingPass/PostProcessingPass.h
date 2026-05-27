#pragma once

#include "DitheringPass.h"
#include "EngineData.h"
#include "FrameData.h"

namespace Graphics {

class PostProcessingPass {
   public:
    PostProcessingPass(EngineData engine_data);

    void render(const FrameData& frame_data);

   private:
    DitheringPass dithering_pass;
};

}  // namespace Graphics