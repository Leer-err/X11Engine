#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>
#include <utility>

template <typename T>
class PoolAllocator {
   public:
    PoolAllocator(size_t max_object_count)
        : data(new uint8_t[max_object_count * sizeof(T)]) {
        first_free_block = data;
        *std::bit_cast<uint8_t**>(first_free_block) = nullptr;
    }
    ~PoolAllocator();

    PoolAllocator& operator=(const PoolAllocator&) = delete;
    PoolAllocator& operator=(PoolAllocator&&) = delete;
    PoolAllocator(const PoolAllocator&) = delete;
    PoolAllocator(PoolAllocator&&) = delete;

    template <typename... ARGS>
    T* create(ARGS&&... args) {
        auto ptr = first_free_block;

        return new (data) T(std::forward<ARGS>(args)...);
    }
    void free(const T* data);

   private:
    uint8_t* getNextFreeBlock(const uint8_t* free_block) const {
        return *std::bit_cast<uint8_t**>(free_block);
    }
    size_t getFreeBlockSize(const uint8_t* free_block) const {
        return *std::bit_cast<size_t*>(free_block + sizeof(uint8_t*));
    }

    uint8_t* data;
    uint8_t* first_free_block;
};