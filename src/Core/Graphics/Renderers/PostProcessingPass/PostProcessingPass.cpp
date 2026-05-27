#include "PostProcessingPass.h"

#include "DitheringPass.h"

namespace Graphics {

PostProcessingPass::PostProcessingPass(EngineData engine_data)
    : dithering_pass(engine_data) {}

void PostProcessingPass::render(const FrameData& frame_data) {
    dithering_pass.render(frame_data);
}

}  // namespace Graphics