#pragma once
#include <vector>
#include <unordered_map>
#include <mutex>

#include "Memory/Memory.h"
#include "EventDelegate.h"
#include "Event.h"

constexpr int EVENT_BUFFER_SIZE = 8192;

using std::vector;
using std::unordered_map;
using std::mutex;
using std::forward;

class EventManager
{
public:
	inline static EventManager* get() {
		static EventManager instance;
		return &instance;
	}

	template<class T, class... Args>
	void RaiseEvent(Args&&... args) {
		void* ptr = m_allocator->allocate(sizeof(T), _alignof(T));

		if (ptr != nullptr) {
			m_mutex.lock();
			m_eventQueue.push_back(new (ptr)T(forward<Args>(args)...));
			m_mutex.unlock();
		}
		else {
			abort();
		}
	}

	void DispatchEvents();

	void AddEventCallback(EventType type, EventDelegate* const eventDelegate);
private:
	EventManager() {
		void* ptr = Memory::GlobalAllocator::get()->allocate(EVENT_BUFFER_SIZE, _alignof(Event));
		m_allocator = new Memory::LinearAllocator(ptr, EVENT_BUFFER_SIZE);
	}
	~EventManager() {
		delete(m_allocator);
	}
	EventManager(EventManager&) = delete;
	EventManager& operator=(EventManager&) = delete;

	mutex m_mutex;

	vector<Event*> m_eventQueue;
	unordered_map<EventType, std::vector<EventDelegate*>> m_callbacks;
	Memory::LinearAllocator* m_allocator;
};