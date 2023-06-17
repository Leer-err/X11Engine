#pragma once
#include "LinearAllocator.h"

#define GLOBAL_MEMORY_ALLOC 1870912

namespace Memory {
	class GlobalAllocator
	{
	public:
		inline static GlobalAllocator* get() {
			static GlobalAllocator instance;
			return &instance;
		}
		void* allocate(size_t size, uint8_t alignment);
	private:
		GlobalAllocator();
		~GlobalAllocator();
		GlobalAllocator(const GlobalAllocator&) = delete;
		GlobalAllocator& operator=(const GlobalAllocator&) = delete;

		LinearAllocator* m_allocator;
		void* m_start;
	};
}