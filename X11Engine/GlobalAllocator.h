#pragma once
#include "LinearAllocator.h"

#define GLOBAL_MEMORY_ALLOC 536870912

namespace Memory {
	class GlobalAllocator
	{
	public:
		static GlobalAllocator& Instance() {
			static GlobalAllocator _instance;
			return _instance;
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