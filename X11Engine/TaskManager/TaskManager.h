#pragma once
#include <thread>
#include <queue>
#include <future>
#include <mutex>
#include <vector>

using std::queue;
using std::vector;
using std::mutex;
using std::future;
using std::promise;
using std::async;
using std::thread;
using std::pair;

class TaskManager {
public:
	static TaskManager& get() {
		static TaskManager instance;
		return instance;
	}

	template<typename F, typename ...ARGS>
	future<void> submit(const F& func, ARGS&&... args) {
		std::lock_guard lock(m_mutex);
		promise<void> completePromise;
		future<void> isComplete = completePromise.get_future();
		m_queue.emplace(async(std::launch::deferred, func, args...), std::move(completePromise));
		return isComplete;
	}
private:
	TaskManager();
	~TaskManager();

	TaskManager(const TaskManager&) = delete;
	TaskManager& operator=(const TaskManager&) = delete;

	void Worker();

	queue<pair<future<void>, promise<void>>> m_queue;

	mutex m_mutex;
	vector<thread> m_threads;
};