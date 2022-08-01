#pragma once
#include "Event/Event.h"
#include "Window.h"

class Mouse {
public:
	inline static Mouse* get() {
		static Mouse instance;
		return &instance;
	}

	void OnMove(const Event* event) {
		const MouseMove* mouseEvent = reinterpret_cast<const MouseMove*>(event);

		x += mouseEvent->x * 3.14f;
		if (x > 3.14f)	 x -= 6.28f;
		if (x < -3.14f) x += 6.28f;

		y += mouseEvent->y * 3.14f;
		if (y > 1.57f) y = 1.57f;
		if (y < -1.57f) y = -1.57f;
	}

	float GetX() const { return x; }
	float GetY() const { return y; }
private:
	Mouse() : x(0.f), y(0.f) {}
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;
	Mouse(const Mouse&&) = delete;
	Mouse& operator=(const Mouse&&) = delete;
	float x, y;
};