#pragma once
#include <vector>
#include <unordered_map>
#include <mutex>

#include "Memory/Memory.h"
#include "EventDelegate.h"
#include "Event.h"

constexpr int EVENT_BUFFER_SIZE = 1024;

using std::vector;
using std::unordered_map;
using std::mutex;

class EventManager
{
public:
	static EventManager* get() {
		static EventManager instance;
		return &instance;
	}

	template<class T, class... Args>
	void RaiseEvent(Args&&... args) {
		void* ptr = m_allocator->allocate(sizeof(T), _alignof(T));

		if (ptr != nullptr) {
			m_mutex.lock();
			m_eventQueue.push_back(new (ptr)T(args...));
			m_mutex.unlock();
		}
		else {
			abort();
		}
	}

	void DispatchEvents() {
		m_mutex.lock();
		for (Event* event : m_eventQueue) {
			auto callbacks = m_callbacks.find(event->GetEventType());
			if (callbacks != m_callbacks.end()) {
				for (auto callback : callbacks->second) {
					callback->invoke(event);
				}
			}
		}
		m_eventQueue.clear();
		m_allocator->clear();
		m_mutex.unlock();
	}

	void AddEventCallback(EventType type, EventDelegate* const eventDelegate) {
		auto callback = m_callbacks.find(type);
		if (callback != m_callbacks.end()) {
			callback->second.push_back(eventDelegate);
		}
		else {
			vector<EventDelegate*> delegates = { eventDelegate };
			m_callbacks.emplace(type, delegates);
		}
	}
private:
	EventManager() {
		void* ptr = Memory::GlobalAllocator::Instance().allocate(EVENT_BUFFER_SIZE, _alignof(Event));
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