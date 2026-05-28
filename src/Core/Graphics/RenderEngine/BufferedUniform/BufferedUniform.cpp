#include "BufferedUniform.h"

#include <cstddef>
#include <cstring>

#include "Buffer.h"
#include "BufferBuilder.h"
#include "Device.h"

namespace Graphics {

BufferedUniformBase::BufferedUniformBase(Device& device, size_t size)
    : size(size) {
    for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        buffers[i] = BufferBuilder(size)
                         .isConstantBuffer()
                         .isCPUWritable(true)
                         .create(device)
                         .getResult();
    }
}

void BufferedUniformBase::update(const FrameData& frame, const void* data) {
    auto ptr = buffers[frame.frame_in_flight_index].mapped_address;

    memcpy(ptr, data, size);
}

uint8_t* BufferedUniformBase::getHostAddress(const FrameData& frame) const {
    return reinterpret_cast<uint8_t*>(
        buffers[frame.frame_in_flight_index].mapped_address);
}

const Buffer& BufferedUniformBase::getBuffer(const FrameData& frame) {
    return buffers[frame.frame_in_flight_index];
}

VkDeviceAddress BufferedUniformBase::getDeviceAddress(const FrameData& frame) {
    return buffers[frame.frame_in_flight_index].device_address;
}

}  // namespace Graphics