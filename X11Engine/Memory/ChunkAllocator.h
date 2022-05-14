#pragma once
#include <list>
#include "PoolAllocator.h"
#include "GlobalAllocator.h"

using std::list;

namespace Memory {
	template<class T, size_t maxObjects>
	class ChunkAllocator
	{
	public:
		class MemoryChunk {
		public:
			MemoryChunk(PoolAllocator* allocator) : m_allocator(allocator) 
			{
				m_chunkStart = m_allocator->GetStart();
				m_chunkEnd = (void*)((char*)m_chunkStart + allocSize);
			}
			list<T*> m_objects;
			inline PoolAllocator* GetAllocator() { return m_allocator; }
			inline void* GetChunkStart() { return m_chunkStart; }
			inline void* GetChunkEnd() { return m_chunkEnd; }
		private:
			PoolAllocator* m_allocator;

			void* m_chunkStart;
			void* m_chunkEnd;
		};

		class iterator : public std::iterator<std::forward_iterator_tag, T> {
		public:
			iterator(typename std::list<MemoryChunk*>::iterator begin, typename std::list<MemoryChunk*>::iterator end) : m_currentChunk(begin), m_end(end) {
				if (begin != end)
				{
					assert((*m_currentChunk) != nullptr);
					m_currentObject = (*m_currentChunk)->m_objects.begin();
				}
				else
				{
					m_currentObject = (*std::prev(m_end))->m_objects.end();
				}
			}

			inline T* Get() { return *m_currentObject; }

			inline iterator& operator++() {
				m_currentObject++;
				if (m_currentObject == (*m_currentChunk)->m_objects.end()) {
					m_currentChunk++;

					if (m_currentChunk != m_end) {
						assert(*m_currentChunk != nullptr);
						m_currentObject = (*m_currentChunk)->m_objects.begin();
					}
				}
				return *this;
			}

			inline T& operator*() const { return *m_currentObject; }
			inline T* operator->() const { return *m_currentObject; }

			inline bool operator==(const iterator& other) { return (m_currentChunk == other.m_currentChunk) && (m_currentObject == other.m_currentObject); }
			inline bool operator!=(const iterator& other) { return (m_currentChunk != other.m_currentChunk) && (m_currentObject != other.m_currentObject); }
		private:
			typename std::list<MemoryChunk*>::iterator m_currentChunk;
			typename std::list<MemoryChunk*>::iterator m_end;
			typename std::list<T*>::iterator m_currentObject;
		};

		ChunkAllocator() {
			PoolAllocator* allocator = new PoolAllocator(sizeof(T), alignof(T), allocSize, GlobalAllocator::Instance().allocate(allocSize, alignof(T)));
			m_chunks.push_back(new MemoryChunk(allocator));
		}

		virtual ~ChunkAllocator() {
			for (auto chunk : m_chunks) {
				for (auto object : chunk->m_objects) {
					((T*)object)->~T();
				}
				chunk->m_objects.clear();
				delete(chunk);
			}
		}

		void* CreateObject() {
			void* slot = nullptr;

			for (MemoryChunk* chunk : m_chunks) {
				if (chunk->m_objects.size() >= maxObjects) {
					continue;
				}

				
				slot = chunk->GetAllocator()->allocate(sizeof(T), alignof(T));
				if (slot != nullptr) {
					chunk->m_objects.push_back((T*)slot);
					break;
				}
			}

			if (slot == nullptr) {
				PoolAllocator* allocator = new PoolAllocator(sizeof(T), alignof(T), allocSize, GlobalAllocator::Instance().allocate(allocSize, alignof(T)));
				MemoryChunk* newChunk = new MemoryChunk(allocator);

				// put new chunk in front
				this->m_chunks.push_front(newChunk);

				slot = newChunk->GetAllocator()->allocate(sizeof(T), alignof(T));

				newChunk->m_objects.push_back((T*)slot);
			}
			return slot;
		}

		void DestroyObject(void* object) {
			void* adr = reinterpret_cast<void*>(object);

			for (auto chunk : this->m_chunks)
			{
				if (chunk->GetChunkStart() <= adr && adr < chunk->GetChunkEnd())
				{
					chunk->m_objects.remove((T*)object);
					chunk->GetAllocator()->free(object);
					return;
				}
			}
		}
		inline iterator begin() { return iterator(this->m_chunks.begin(), this->m_chunks.end()); }
		inline iterator end() { return iterator(this->m_chunks.end(), this->m_chunks.end()); }
	protected:
		std::list<MemoryChunk*> m_chunks;
	private:
		static const size_t allocSize = sizeof(T) * maxObjects;
	};
}