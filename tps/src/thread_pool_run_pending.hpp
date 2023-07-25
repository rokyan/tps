#pragma once

#include "task.hpp"
#include "threadsafe_queue.hpp"

#include <future>

namespace tps
{

class thread_pool_run_pending final
{
public:
    thread_pool_run_pending();

    thread_pool_run_pending(const thread_pool_run_pending&) = delete;
    thread_pool_run_pending& operator=(const thread_pool_run_pending&) = delete;

    thread_pool_run_pending(thread_pool_run_pending&&) = default;
    thread_pool_run_pending& operator=(thread_pool_run_pending&&) = default;

    template<typename Func>
    auto submit(Func func);

    void run_pending();

    ~thread_pool_run_pending();

private:
    void run();

private:
    std::atomic_bool done;
    threadsafe_queue<task> queue;
    std::vector<std::jthread> threads;
};

template<typename Func>
auto thread_pool_run_pending::submit(Func func)
{
    using ret_type = decltype(func());

    std::packaged_task<ret_type()> task{ std::move(func) };
    auto future = task.get_future();

    queue.push(std::move(task));

    return future;
}

}
