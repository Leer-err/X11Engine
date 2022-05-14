#include "LinearAllocator.h"

namespace Memory {
	void* LinearAllocator::allocate(size_t size, uint8_t alignment) {
		uint8_t adjustment = Memory::alignAdjustment(_currPos, alignment);
		if (_used + alignment + size > _size) return nullptr;
		void* aligned_address = reinterpret_cast<void*>(reinterpret_cast<uint64_t>(_currPos) + adjustment);
		_currPos = reinterpret_cast<void*>(reinterpret_cast<uint64_t>(aligned_address) + size);
		_used += size + adjustment;
		return aligned_address;
	}

	void LinearAllocator::clear() {
		_currPos = _start;
		_used = 0;

	}

	void LinearAllocator::free(void* ptr) {
		throw;
	}
}