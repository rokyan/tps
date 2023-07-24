#pragma once

#include "threadsafe_queue.hpp"
#include <functional>
#include <atomic>
#include <thread>
#include <vector>

namespace tps
{

class thread_pool_simple final
{
private:
    using task_type = std::function<void()>;

public:
    thread_pool_simple();

    thread_pool_simple(const thread_pool_simple&) = delete;
    thread_pool_simple& operator=(const thread_pool_simple&) = delete;

    thread_pool_simple(thread_pool_simple&&) = default;
    thread_pool_simple& operator=(thread_pool_simple&&) = default;

    void submit(task_type task);

    ~thread_pool_simple();

private:
    void run();

private:
    std::atomic_bool done;
    threadsafe_queue<task_type> queue;
    std::vector<std::jthread> threads;
};


}
