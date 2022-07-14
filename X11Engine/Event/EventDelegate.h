#pragma once
#include <functional>

using std::function;
using std::forward;
using std::bind;

struct Event;

class EventDelegate {
public:
	template<typename F,typename... ARGS>
	EventDelegate(F&& fun, ARGS&&... args) {
		m_callback = bind(fun, forward<ARGS...>(args...), std::placeholders::_1);
	}
	inline void invoke(const Event* event) { m_callback(event); }
private:
	function<void(const Event*)> m_callback;
};