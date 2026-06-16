#pragma once

#include <array>
#include <unordered_map>

#include "Buffer.h"
#include "Device.h"
#include "EngineConstants.h"

namespace Graphics {

class BufferRegistry {
    using BufferKey = uint32_t;

   public:
    explicit BufferRegistry(Device& device);
    ~BufferRegistry();

    BufferRegistry(const BufferRegistry&) = delete;
    BufferRegistry& operator=(const BufferRegistry&) = delete;
    BufferRegistry(BufferRegistry&&) = delete;
    BufferRegistry& operator=(BufferRegistry&&) = delete;

    Buffer registerBuffer(const BufferState& buffer);
    Buffer registerBufferChain(
        const std::array<BufferState, MAX_FRAMES_IN_FLIGHT>& buffer);

    BufferState getState(BufferHandle handle) const;

    void setFrameInFlight(uint32_t frame_index);

   private:
    using BufferChain = std::array<BufferKey, MAX_FRAMES_IN_FLIGHT>;

    Device& device;

    std::unordered_map<BufferKey, BufferState> buffers;
    std::unordered_map<BufferHandle, BufferChain> buffer_chains;
    std::unordered_map<BufferHandle, bool> buffer_chain_flags;

    BufferHandle next_handle = 1;
    BufferKey next_key = 1;

    uint32_t frame_index = 0;
};

}  // namespace Graphics