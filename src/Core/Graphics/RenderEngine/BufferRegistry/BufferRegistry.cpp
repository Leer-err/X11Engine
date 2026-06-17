#include "BufferRegistry.h"

#include <array>

#include "Buffer.h"
#include "EngineConstants.h"

namespace Graphics {

BufferHandle BufferRegistry::registerBuffer(RawBufferHandle buffer) {
    auto handle = next_handle++;

    buffer_chains.try_emplace(handle, BufferChain{buffer});
    buffer_chain_flags.try_emplace(handle, false);

    return handle;
}

BufferHandle BufferRegistry::registerBufferChain(
    const std::array<RawBufferHandle, MAX_FRAMES_IN_FLIGHT>& buffers) {
    auto handle = next_handle++;

    buffer_chains.try_emplace(handle, buffers);
    buffer_chain_flags.try_emplace(handle, true);

    return handle;
}

RawBufferHandle BufferRegistry::getRawBuffer(BufferHandle handle) const {
    auto chain_it = buffer_chains.find(handle);
    if (chain_it == buffer_chains.end()) return {};
    auto& chain = chain_it->second;

    bool is_chained = buffer_chain_flags.at(handle);
    if (!is_chained)
        return chain[0];
    else
        return chain[frame_index];
}

void BufferRegistry::setFrameInFlight(uint32_t index) { frame_index = index; }

}  // namespace Graphics