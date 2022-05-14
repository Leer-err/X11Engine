#pragma once

#include "Allocator.h"

namespace Memory {
	class LinearAllocator : public Allocator
	{
	public:
		LinearAllocator(void* start, size_t size) : Allocator(start, size), _currPos(start) {};
		void* allocate(size_t size, uint8_t alignment) override;
		void free(void* ptr) override;
		void clear();
	private:
		void* _currPos;
	};
}