#pragma once
#include <stdint.h>

enum class EventType {
	None = 0,
	KeyDown = 1,
	KeyUp = 2,
	MouseMove = 4,
};

struct Event {
public:
	Event(EventType type) : m_type(type) {};
	EventType GetEventType() const { return m_type; }
protected:
	EventType m_type;
};

struct KeyDown : public Event {
public:
	KeyDown(int key) : keyCode(key), Event(EventType::KeyDown) {}
	uint64_t keyCode;
};

struct KeyUp : public Event {
public:
	KeyUp(int key) : keyCode(key), Event(EventType::KeyUp) {}
	uint64_t keyCode;
};

struct MouseMove : public Event {
	MouseMove(float x, float y) : x(x), y(y), Event(EventType::MouseMove) {}
	float x, y;
};

constexpr int MAX_EVENT_SIZE = sizeof(MouseMove);