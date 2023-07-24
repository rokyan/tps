#pragma once

#include "move_only_task.hpp"
#include "threadsafe_queue.hpp"
#include <future>
#include <vector>
#include <atomic>
#include <thread>

namespace tps
{

class thread_pool_waitable_task final
{
private:
    using task_type = move_only_task;

public:
    thread_pool_waitable_task();

    thread_pool_waitable_task(const thread_pool_waitable_task&) = delete;
    thread_pool_waitable_task& operator=(const thread_pool_waitable_task&) = delete;

    thread_pool_waitable_task(thread_pool_waitable_task&&) = default;
    thread_pool_waitable_task& operator=(thread_pool_waitable_task&&) = default;

    template<typename Func>
    auto submit(Func func);

    ~thread_pool_waitable_task();

private:
    void run();

private:
    std::atomic_bool done;
    threadsafe_queue<task_type> queue;
    std::vector<std::jthread> threads;
};

template<typename Func>
auto thread_pool_waitable_task::submit(Func func)
{
    using ret_type = decltype(func());

    std::packaged_task<ret_type()> task{ std::move(func) };
    auto future = task.get_future();

    queue.push(std::move(task));

    return future;
}

}
