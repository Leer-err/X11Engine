#pragma once

#include "Allocator.h"
#include "ChunkAllocator.h"
#include "GlobalAllocator.h"
#include "LinearAllocator.h"
#include "StackAllocator.h"
#include "PoolAllocator.h"


constexpr size_t CHUNK_SIZE = 512;

namespace Memory {
	template<class T> T* allocate(Allocator& allocator) {
		return new (allocator.allocate(sizeof(T), __alignof(T))) T;
	}

	template <class T> T* allocateNew(Allocator& allocator, const T& object)
	{
		return new (allocator.allocate(sizeof(T), __alignof(T))) T(object);
	}

	template<class T> void free(Allocator& allocator, T& object) {
		object.~T();
		allocator.free(&object);
	}
}

