#include "BufferRegistry/BufferAllocator.h"
#include <vma/vk_mem_alloc.h>
#include <iostream> // For basic error logging if needed, though not strictly required by header

namespace Graphics {

// Constructor implementation
BufferAllocator::BufferAllocator(VmaAllocator allocator) : allocator(allocator) {}

// Destructor implementation: clean up all allocated buffers
BufferAllocator::~BufferAllocator() {
    for (const auto& [handle, state] : buffers) {
        if (state.buffer != VK_NULL_HANDLE && state.allocation != nullptr) {
            vmaDestroyBuffer(allocator, state.buffer, state.allocation);
        }
    }
}

// Creates a buffer using VMA and registers it
RawBufferHandle BufferAllocator::createBuffer(const VkBufferCreateInfo& buffer_info, const VmaAllocationCreateInfo& alloc_info) {
    VkBuffer buffer = VK_NULL_HANDLE;
    VmaAllocation allocation = nullptr;

    // Use vmaCreateBuffer to allocate and create the buffer in one go
    VkResult result = vmaCreateBuffer(allocator, &buffer_info, &alloc_info, &buffer, &allocation, nullptr);

    if (result != VK_SUCCESS) {
        std::cerr << "ERROR: Failed to create VMA buffer! Result: " << result << std::endl;
        return 0; // Return invalid handle or throw exception
    }

    RawBufferHandle handle = next_handle++;
    
    // Assuming BufferState needs VkBuffer, VmaAllocation* (or equivalent), and possibly size/usage flags.
    // Since the header defines `BufferState` but doesn't show its structure, I will infer it based on usage context:
    // It must hold enough info to destroy later (VkBuffer and VmaAllocation).

    // NOTE: If BufferState is not fully defined in scope for this implementation file, 
    // we rely on the definition provided by "BufferState.h" which was mentioned but not seen.
    // Assuming BufferState contains members matching VkBuffer and VmaAllocation*.
    BufferState state;
    state.buffer = buffer;
    state.allocation = allocation;
    // Add other relevant state fields here if necessary (e.g., size, usage)

    buffers[handle] = state;

    return handle;
}

// Retrieves the stored state for a given handle
std::optional<BufferState> BufferAllocator::getBufferState(RawBufferHandle handle) const {
    if (buffers.count(handle)) {
        return buffers.at(handle);
    }
    return std::nullopt;
}

    // Deletes the buffer associated with a handle
    void BufferAllocator::deleteBuffer(RawBufferHandle handle) {
        auto it = buffers.find(handle);
        if (it == buffers.end()) {
            std::cerr << "WARNING: Attempted to delete unknown buffer handle: " << handle << std::endl;
            return;
        }

        const BufferState& state = it->second;
        if (state.buffer != VK_NULL_HANDLE) {
            vmaDestroyBuffer(allocator, state.buffer, state.allocation);
        }

        buffers.erase(it);
    }

}  // namespace Graphics