#include "PostProcessingPass.h"

#include "Device.h"
#include "DitheringPass.h"

namespace Graphics {

PostProcessingPass::PostProcessingPass(Device& device,
                                       const EngineData& engine_data)
    : dithering_pass(device, engine_data) {}

void PostProcessingPass::render(const FrameData& frame_data) {
    dithering_pass.render(frame_data);
}

}  // namespace Graphics