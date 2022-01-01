#pragma once

enum class EventType {
	None = 0,
	KeyDown = 1,
	KeyUp = 2,
	MouseMove = 4,
};

class Event {
public:
	Event(EventType type) : m_handeled(false), m_type(type) {};
	virtual EventType GetEventType() const = 0;
protected:
	EventType m_type;
	bool m_handeled;
};

class KeyDown : public Event {
public:
	KeyDown(int key) : keyCode(key), Event(EventType::KeyDown) {}
	EventType GetEventType() const override { return m_type; }
	uint64_t keyCode;
};

class KeyUp : public Event {
public:
	KeyUp(int key) : keyCode(key), Event(EventType::KeyUp) {}
	EventType GetEventType() const override { return m_type; }
	uint64_t keyCode;
};

class MouseMove : public Event {
public:
	MouseMove(int x, int y) : x(x), y(y), Event(EventType::MouseMove) {}
	EventType GetEventType() const override { return m_type; }
	int x;
	int y;
};