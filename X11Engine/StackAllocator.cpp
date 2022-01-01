#include "StackAllocator.h"

namespace Memory {
	StackAllocator::StackAllocator(void* start, size_t size) : Allocator(start, size), _currPos(start)
	{
#if _DEBUG 
		prev_position = nullptr;
#endif 
	}

	void* StackAllocator::allocate(size_t size, uint8_t alignment) {
		uint8_t adjustment = alignAdjustmentHeader(_currPos, alignment, sizeof(AllocationHeader));
		if (reinterpret_cast<uint64_t>(_currPos) + size + adjustment > _size) return nullptr;

		void* alignedAddress = reinterpret_cast<void*>(reinterpret_cast<uint64_t>(_currPos) + adjustment);
		AllocationHeader* header = reinterpret_cast<AllocationHeader*>(reinterpret_cast<uint64_t>(alignedAddress) - sizeof(AllocationHeader));
		header->adjustment = adjustment;

#if _DEBUG 
		header->prev_address = prev_position;
		prev_position = alignedAddress;
#endif 

		_currPos = reinterpret_cast<void*>(reinterpret_cast<uint64_t>(alignedAddress) + size);
		_used += size + adjustment;

		return alignedAddress;
	}

	void StackAllocator::free(void* ptr) {
#if _DEBUG
		if (ptr != prev_position) {
			throw "Trying to free address behind last allocation";
		}
#endif
		AllocationHeader* header = reinterpret_cast<AllocationHeader*>(reinterpret_cast<uint64_t>(ptr) - sizeof(AllocationHeader));

		_used -= reinterpret_cast<uint64_t>(_currPos) - reinterpret_cast<uint64_t>(ptr) + header->adjustment;
		_currPos = reinterpret_cast<void*>(reinterpret_cast<uint64_t>(ptr) - header->adjustment);

#if _DEBUG 
		prev_position = header->prev_address;
#endif
	}
}