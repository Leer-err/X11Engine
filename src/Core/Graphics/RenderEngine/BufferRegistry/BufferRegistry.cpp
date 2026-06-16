#include "BufferRegistry.h"

#include <array>

#include "EngineConstants.h"

namespace Graphics {

BufferRegistry::BufferRegistry(Device& device) : device(device) {}

BufferRegistry::~BufferRegistry() {
    for (const auto& [key, state] : buffers) {
        device.destroyBuffer(state);
    }
}

Buffer BufferRegistry::registerBuffer(const BufferState& buffer) {
    auto key = next_key++;
    auto handle = next_handle++;

    buffers.try_emplace(key, buffer);
    buffer_chains.try_emplace(handle, BufferChain{key});
    buffer_chain_flags.try_emplace(handle, false);

    return Buffer(handle);
}

Buffer BufferRegistry::registerBufferChain(
    const std::array<BufferState, MAX_FRAMES_IN_FLIGHT>& buffer) {
    auto keys = BufferChain();
    auto handle = next_handle++;

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        auto key = next_key++;

        keys[i] = key;
        buffers.try_emplace(key, buffer);
    }

    buffer_chains.try_emplace(handle, keys);
    buffer_chain_flags.try_emplace(handle, true);

    return Buffer(handle);
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

    return buffers[key];
}

void BufferRegistry::setFrameInFlight(uint32_t index) { frame_index = index; }

}  // namespace Graphics