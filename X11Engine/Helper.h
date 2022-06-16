#pragma once
#include <Windows.h>
#include "Types.h"

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