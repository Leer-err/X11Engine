#pragma once
#include "Allocator.h"

namespace Memory {
	class PoolAllocator : public Allocator
	{
	public:
		PoolAllocator(size_t objectSize, uint8_t alignment, size_t size, void* start);
		~PoolAllocator();
		void* allocate(size_t size, uint8_t alignment) override;
		void free(void* ptr) override;
	private:
		uint32_t _objectSize;
		uint8_t _alignment;
		void** _freeList;
	};
}