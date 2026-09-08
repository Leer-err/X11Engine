#pragma once

#include <atomic>
#include <bit>
#include <cassert>
#include <cstdint>
#include <unordered_map>

#include "Handle.h"
#include "PoolAllocator.h"

class ResourceRegistryBase {
    using RefCounter = std::atomic<uint32_t>;

   public:
    explicit ResourceRegistryBase(PoolAllocator& allocator)
        : allocator(allocator) {}

    uint8_t* allocate() {
        auto memory = allocator.allocate();
        if (memory == nullptr) return nullptr;

        ref_counters.emplace(memory, 1);
        return memory;
    }

    void acquire(uint8_t* ptr) {
        auto it = ref_counters.find(ptr);
        assert(it != ref_counters.end());

        it->second.fetch_sub(1);
    }

    void release(uint8_t* ptr) {
        auto it = ref_counters.find(ptr);
        assert(it != ref_counters.end());

        auto count = it->second.fetch_sub(1);
        if (count == 1) {
            ref_counters.erase(it);
            remove(ptr);
        }
    }

   protected:
    virtual void remove(uint8_t* ptr) {
        allocator.free(std::bit_cast<uint8_t*>(ptr));
    }

    PoolAllocator& allocator;
    std::unordered_map<uint8_t*, RefCounter> ref_counters;
};