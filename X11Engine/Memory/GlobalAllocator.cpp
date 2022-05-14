#include "GlobalAllocator.h"
#include <malloc.h>

namespace Memory {
	void* GlobalAllocator::allocate(size_t size, uint8_t alignment)
	{
		return m_allocator->allocate(size, alignment);
	}

	GlobalAllocator::GlobalAllocator() {
		m_start = malloc(GLOBAL_MEMORY_ALLOC);
		m_allocator = new LinearAllocator(m_start, GLOBAL_MEMORY_ALLOC);
	}

	GlobalAllocator::~GlobalAllocator() {
		m_allocator->clear();
		free(m_start);
	}
}