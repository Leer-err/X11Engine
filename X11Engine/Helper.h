#pragma once
#include <Windows.h>
#include "Types.h"

inline void ThrowIfFailed(HRESULT hr) {
	if (!SUCCEEDED(hr)) throw;
}

template<class T>
class Helper {
public:
	template<class U>
	static const TypeId Get() {
		static const TypeId TYPE_ID{ s_count++ };
		return TYPE_ID;
	}

	static const TypeId Get() {
		return s_count;
	}
private:
	static TypeId s_count;
};

template<class T>
TypeId Helper<T>::s_count = 0;