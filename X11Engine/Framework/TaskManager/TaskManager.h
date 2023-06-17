#pragma once
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

using std::bind;
using std::function;
using std::future;
using std::make_shared;
using std::move;
using std::mutex;
using std::packaged_task;
using std::queue;
using std::shared_ptr;
using std::thread;
using std::vector;

class TaskManager {
   public:
    inline static TaskManager* get() {
        static TaskManager instance;
        return &instance;
    }

    template <typename F, typename... ARGS>
    future<std::invoke_result_t<F, ARGS...>> submit(F&& func, ARGS&&... args) {
        using return_t = std::invoke_result_t<F, ARGS...>;
        using task_t = packaged_task<return_t()>;

        auto bindFunc = bind(forward<F>(func), forward<ARGS>(args)...);
        shared_ptr<task_t> task = make_shared<task_t>(move(bindFunc));
        future<return_t> result = task->get_future();

        m_mutex.lock();
        m_queue.emplace([task]() -> void { (*task)(); });
        m_mutex.unlock();
        return result;
    }

   private:
    TaskManager();
    ~TaskManager();

    TaskManager(const TaskManager&) = delete;
    TaskManager& operator=(const TaskManager&) = delete;

    void Worker();

    queue<function<void()>> m_queue;

    mutex m_mutex;
    vector<thread> m_threads;
};