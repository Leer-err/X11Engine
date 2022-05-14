#pragma once
#include <cstdint>

namespace Memory {
	class Allocator
	{
	public:
		Allocator(void* start, size_t size) : _start(start), _size(size), _used(0) {};

		inline void* GetStart() { return _start; }

		virtual void* allocate(size_t size, uint8_t alignment) = 0;
		virtual void free(void* ptr) = 0;
	protected:
		void* _start;
		size_t _size;
		size_t _used;
	};


	inline void* align(void* ptr, uint8_t alignment) {
		return reinterpret_cast<void*>(reinterpret_cast<uint64_t>(reinterpret_cast<uint8_t*>(ptr) + static_cast<uint8_t>(alignment - 1)) & static_cast<uint8_t>(~(alignment - 1)));
	}

	inline uint8_t alignAdjustment(const void* ptr, uint8_t alignment) {
		uint8_t adjustment = alignment - (reinterpret_cast<uint64_t>(ptr) & static_cast<uint64_t>(alignment - 1));
		if (alignment == adjustment) return 0;

		return adjustment;
	}

	inline uint8_t alignAdjustmentHeader(const void* ptr, uint8_t alignment, uint8_t headerSize) {
		uint8_t adjustment = alignAdjustment(ptr, alignment);

		if (adjustment < headerSize) {
			adjustment -= headerSize;
			adjustment += alignment * (headerSize / alignment);

			if (headerSize % alignment) {
				adjustment += alignment;
			}
		}

		return adjustment;
	}
}