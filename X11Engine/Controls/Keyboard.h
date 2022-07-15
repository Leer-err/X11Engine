#pragma once
#include <vector>
#include "Event/Event.h"

using std::vector;

class Keyboard {
public:
	static Keyboard* get() {
		static Keyboard instance;
		return &instance;
	}

	void OnKeyDown(const Event* event) {
		m_keys[((KeyDown*)event)->keyCode] = true;
	}
	void OnKeyUp(const Event* event) {
		m_keys[((KeyDown*)event)->keyCode] = false;
	}

	bool IsPressed(const char code) {
		return m_keys[code];
	}
private:
	Keyboard() {
		memset(m_keys, 0, sizeof(m_keys));
	}

	bool m_keys[0xfe];
};