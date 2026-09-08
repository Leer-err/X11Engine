#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>

template <typename T>
T align(const T data, size_t alignment) {
    auto tmp = std::bit_cast<size_t>(data + alignment - 1);

    return std::bit_cast<T>(tmp - tmp % alignment);
}

class PoolAllocator {
    struct Node {
        Node* next;
    };

   public:
    PoolAllocator(uint8_t* ptr, size_t pool_size, size_t object_size,
                  size_t alignment);

    uint8_t* allocate();
    void free(const uint8_t* data);

    PoolAllocator& operator=(const PoolAllocator&) = delete;
    PoolAllocator& operator=(PoolAllocator&&) = delete;
    PoolAllocator(const PoolAllocator&) = delete;
    PoolAllocator(PoolAllocator&&) = delete;

   private:
    const uint8_t* ptr;
    size_t pool_size;
    Node* free_list;
    size_t aligned_object_size;
};