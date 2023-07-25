#pragma once

#include "threadsafe_queue.hpp"
#include "task.hpp"

#include <future>
#include <atomic>
#include <memory>
#include <queue>

namespace tps
{

class thread_pool_local_queue final
{
public:
    thread_pool_local_queue();

    thread_pool_local_queue(const thread_pool_local_queue&) = delete;
    thread_pool_local_queue& operator=(const thread_pool_local_queue&) = delete;

    thread_pool_local_queue(thread_pool_local_queue&&) = default;
    thread_pool_local_queue& operator=(thread_pool_local_queue&&) = default;

    template<typename Func>
    auto submit(Func func);

    void run_pending();

    ~thread_pool_local_queue();

private:
    void run();

private:
    std::atomic_bool done;
    threadsafe_queue<task> queue;
    std::vector<std::jthread> threads;
    static thread_local std::unique_ptr<std::queue<task>> local_queue;
};

template<typename Func>
auto thread_pool_local_queue::submit(Func func)
{
    std::packaged_task<decltype(func())()> task{ std::move(func) };

    auto future = task.get_future();

    if (local_queue != nullptr)
    {
        local_queue->push(std::move(task));
    }
    else
    {
        queue.push(std::move(task));
    }

    return future;
}

}
