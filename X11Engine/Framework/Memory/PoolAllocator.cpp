#include "PoolAllocator.h"

Memory::PoolAllocator::PoolAllocator(size_t objectSize, uint8_t alignment, size_t size, void* start) : Allocator(start, size), _alignment(alignment), _objectSize(objectSize)
{
	uint8_t adjustment = alignAdjustment(start, alignment);
	_freeList = reinterpret_cast<void**>(reinterpret_cast<uint64_t>(start) + adjustment);
	uint32_t numObjects = size / objectSize;
	void** p = _freeList;

	for (int i = 0; i < numObjects; i++) {
		*p = p + objectSize;
		p = reinterpret_cast<void**>(*p);
	}
	*p = nullptr;
}

Memory::PoolAllocator::~PoolAllocator()
{
	_freeList = nullptr;
}

void* Memory::PoolAllocator::allocate(size_t size, uint8_t alignment)
{
	if (_freeList == nullptr) { return nullptr; }

	void* p = _freeList;
	_freeList = reinterpret_cast<void**>(*_freeList);
	_used += size;
	return p;
}

void Memory::PoolAllocator::free(void* ptr)
{
	*((void**)ptr) = _freeList;
	_freeList = (void**)ptr;
	_used -= _objectSize;
}