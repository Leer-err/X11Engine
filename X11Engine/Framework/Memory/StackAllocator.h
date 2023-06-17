#pragma once
#include "Allocator.h"

namespace Memory {
    class StackAllocator : public Allocator
    {
    public:
        StackAllocator(void* start, size_t size);
        void* allocate(size_t size, uint8_t alignment) override;
        void free(void* ptr) override;
    private:
        void* _currPos;
        struct AllocationHeader {
            uint8_t adjustment;
#if _DEBUG 
            void* prev_address;
#endif
        };
#if _DEBUG 
        void* prev_position;
#endif
    };
}
