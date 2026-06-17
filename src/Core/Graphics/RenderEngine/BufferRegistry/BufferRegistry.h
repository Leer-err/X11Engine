#pragma once

#include <array>
#include <unordered_map>

#include "Buffer.h"
#include "BufferAllocator.h"
#include "EngineConstants.h"

namespace Graphics {

class BufferRegistry {
   public:
    BufferHandle registerBuffer(RawBufferHandle buffer);
    BufferHandle registerBufferChain(
        const std::array<RawBufferHandle, MAX_FRAMES_IN_FLIGHT>& buffers);

    RawBufferHandle getRawBuffer(BufferHandle handle) const;

    void setFrameInFlight(uint32_t frame_index);

   private:
    using BufferChain = std::array<RawBufferHandle, MAX_FRAMES_IN_FLIGHT>;

    std::unordered_map<BufferHandle, BufferChain> buffer_chains;
    std::unordered_map<BufferHandle, bool> buffer_chain_flags;

    BufferHandle next_handle = 1;

    uint32_t frame_index = 0;
};

}  // namespace Graphics