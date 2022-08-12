#include "TaskManager.h"

#include "Window.h"

TaskManager::TaskManager() {
    int num_threads = thread::hardware_concurrency();
    for (int i = 0; i < num_threads; i++) {
        m_threads.emplace_back(&TaskManager::Worker, this);
    }
}

TaskManager::~TaskManager() {
    for (auto& thread : m_threads) {
        thread.join();
    }
}

void TaskManager::Worker() {
    while (Window::get()->IsRunning() || !m_queue.empty()) {
        m_mutex.lock();
        if (!m_queue.empty()) {
            auto func = move(m_queue.front());
            m_queue.pop();
            m_mutex.unlock();

            func();
            continue;
        }
        m_mutex.unlock();
        std::this_thread::yield();
    }
}