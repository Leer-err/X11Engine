#pragma once
#include "Event.h"

class IEventDelegate {
public:
	virtual inline void invoke(const Event* const) = 0;
};

template<class Class, class EventClass>
class EventDelegate : public IEventDelegate {
	typedef void(Class::* Callback)(const Event*);
public:
	EventDelegate(Class* reciever, Callback callback) : m_reciever(reciever), m_callback(callback) { }
	inline void invoke(const Event* event) override { (m_reciever->*m_callback)(event); }
private:
	Class* m_reciever;
	Callback m_callback;
};