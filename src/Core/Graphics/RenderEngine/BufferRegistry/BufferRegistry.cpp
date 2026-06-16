#include "BufferRegistry.h"

#include <array>

#include "Buffer.h"
#include "EngineConstants.h"

namespace Graphics {

BufferRegistry::BufferRegistry(VmaAllocator allocator) : allocator(allocator) {}

BufferRegistry::~BufferRegistry() {
    for (const auto& [key, state] : buffers) {
        vmaDestroyBuffer(allocator, state.buffer, state.allocation);
    }
}

Buffer BufferRegistry::registerBuffer(const BufferState& buffer) {
    auto key = next_key++;
    auto handle = next_handle++;

    buffers.try_emplace(key, buffer);
    buffer_chains.try_emplace(handle, BufferChain{key});
    buffer_chain_flags.try_emplace(handle, false);

    return Buffer(this, handle);
}

Buffer BufferRegistry::registerBufferChain(
    const std::array<BufferState, MAX_FRAMES_IN_FLIGHT>& buffer) {
    auto keys = BufferChain();
    auto handle = next_handle++;

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        auto key = next_key++;

        keys[i] = key;
        buffers.try_emplace(key, buffer[i]);
    }

    buffer_chains.try_emplace(handle, keys);
    buffer_chain_flags.try_emplace(handle, true);

    return Buffer(this, handle);
}

BufferState BufferRegistry::getState(BufferHandle handle) const {
    auto chain_it = buffer_chains.find(handle);
    if (chain_it == buffer_chains.end()) return {};

    auto& chain = chain_it->second;

    bool is_chained = buffer_chain_flags.at(handle);
    BufferKey key;
    if (!is_chained)
        key = chain[0];
    else
        key = chain[frame_index];

    return buffers.at(key);
}

void BufferRegistry::setFrameInFlight(uint32_t index) { frame_index = index; }

}  // namespace Graphics