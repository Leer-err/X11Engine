#pragma once

#include <sys/types.h>
#include <vulkan/vulkan_core.h>

#include <array>
#include <cstddef>
#include <cstdint>

#include "Buffer.h"
#include "Device.h"
#include "EngineConstants.h"
#include "EngineData.h"
#include "FrameData.h"

namespace Graphics {

class BufferedUniformBase {
   public:
    BufferedUniformBase(Device& device, size_t size);

    void update(const FrameData& frame, const void* data);

    uint8_t* getHostAddress(const FrameData& frame) const;

    const Buffer& getBuffer(const FrameData& frame);
    VkDeviceAddress getDeviceAddress(const FrameData& frame);

   private:
    std::array<Buffer, MAX_FRAMES_IN_FLIGHT> buffers;
    size_t size;
};

template <typename T>
class BufferedUniform {
   public:
    BufferedUniform(Device& device) : base(device, sizeof(T)) {}

    void update(const FrameData& frame, const T& data) {
        base.update(frame, &data);
    }

    const Buffer& getBuffer(const FrameData& frame) {
        return base.getBuffer(frame);
    }

    T* getHostAddress(const FrameData& frame) const {
        return std::bit_cast<T*>(base.getHostAddress(frame));
    }

    VkDeviceAddress getDeviceAddress(const FrameData& frame) {
        return base.getDeviceAddress(frame);
    }

   private:
    BufferedUniformBase base;
};

}  // namespace Graphics