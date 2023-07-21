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

    void submit(task_type task);

    ~thread_pool_simple();

private:
    void run();

private:
    std::atomic_bool done;
    threadsafe_queue<task_type> queue;
    std::vector<std::jthread> threads;
};

thread_pool_simple::thread_pool_simple()
    : done{ false }
{
    for (auto hc = std::thread::hardware_concurrency(); hc--; )
    {
        threads.emplace_back(&thread_pool_simple::run, this);
    }
}

void thread_pool_simple::submit(task_type task)
{
    queue.push(std::move(task));
}

void thread_pool_simple::run()
{
    while (!done.load())
    {
        task_type task;

        if (queue.try_get(task))
        {
            task();
        }
        else
        {
            std::this_thread::yield();
        }
    }
}

thread_pool_simple::~thread_pool_simple()
{
    done.store(true);
}

}
