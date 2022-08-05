#pragma once
#include <vector>
#include <unordered_map>
#include <mutex>

#include "Memory/Memory.h"
#include "EventDelegate.h"
#include "Event.h"

constexpr int EVENT_BUFFER_SIZE = 0x2000;
constexpr int MAX_EVENT_CALLBACKS = 0x10;

using std::forward;
using std::move;
using std::lock_guard;
using std::mutex;
using std::unordered_map;
using std::vector;

class EventManager
{
public:
	inline static EventManager *get()
	{
		static EventManager instance;
		return &instance;
	}

	template <class T, class... Args>
	void RaiseEvent(Args &&...args)
	{
		lock_guard lock(m_eventMutex);
		void *ptr = m_eventAllocator->allocate(sizeof(T), _alignof(T));

		if (ptr != nullptr)
		{
			m_eventQueue.push_back(new (ptr) T(forward<Args>(args)...));
		}
	}

	void DispatchEvents();

	template<typename F, typename... ARGS>
	void AddEventCallback(EventType type, F&& fun, ARGS&&... args)
	{
		lock_guard lock(m_callbackMutex);
		void *ptr = m_callbackAllocator->allocate(sizeof(EventDelegate), _alignof(EventDelegate));

		if (ptr != nullptr)
		{
			EventDelegate* delegate = new (ptr)EventDelegate(move(fun), forward<ARGS>(args)...);
			auto callbacks = m_callbacks.find(type);
			if (callbacks != m_callbacks.end())
			{
				callbacks->second.emplace_back(delegate);
			}
			else
			{
				vector<EventDelegate *> delegates = {delegate};
				m_callbacks.emplace(type, delegates);
			}
		}
	}

private:
	EventManager()
	{
		void *eventAllocator = Memory::GlobalAllocator::get()->allocate(EVENT_BUFFER_SIZE * MAX_EVENT_SIZE, _alignof(Event));
		void *callbackAllocator = Memory::GlobalAllocator::get()->allocate(sizeof(EventDelegate) * MAX_EVENT_CALLBACKS, _alignof(Event));
		m_eventAllocator = new Memory::LinearAllocator(eventAllocator, EVENT_BUFFER_SIZE * MAX_EVENT_SIZE);
		m_callbackAllocator = new Memory::LinearAllocator(callbackAllocator, sizeof(EventDelegate) * MAX_EVENT_CALLBACKS);
	}
	~EventManager() = default;
	EventManager(EventManager &) = delete;
	EventManager &operator=(EventManager &) = delete;

	mutex m_eventMutex, m_callbackMutex;

	vector<Event *> m_eventQueue;
	unordered_map<EventType, std::vector<EventDelegate *>> m_callbacks;
	Memory::LinearAllocator *m_eventAllocator;
	Memory::LinearAllocator *m_callbackAllocator;
};