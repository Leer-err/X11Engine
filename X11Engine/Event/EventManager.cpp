#include "EventManager.h"

void EventManager::DispatchEvents()
{
	m_mutex.lock();
	for (Event *event : m_eventQueue)
	{
		auto callbacks = m_callbacks.find(event->GetEventType());
		if (callbacks != m_callbacks.end())
		{
			for (auto callback : callbacks->second)
			{
				callback->invoke(event);
			}
		}
	}
	m_eventQueue.clear();
	m_allocator->clear();
	m_mutex.unlock();
}

void EventManager::AddEventCallback(EventType type, EventDelegate *const eventDelegate)
{
	auto callback = m_callbacks.find(type);
	if (callback != m_callbacks.end())
	{
		callback->second.push_back(eventDelegate);
	}
	else
	{
		vector<EventDelegate *> delegates = {eventDelegate};
		m_callbacks.emplace(type, delegates);
	}
}