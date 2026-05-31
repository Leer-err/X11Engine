#pragma once

#include "CommandBuffer.h"
#include "Device.h"

namespace Graphics {

struct FrameData {
    uint32_t frame_in_flight_index;

    CommandBuffer cmd;
};

}  // namespace Graphics