#pragma once

#include "move_only_task.hpp"

#include <future>

namespace tps
{

class thread_pool_run_pending final
{
private:
    using task_type = move_only_task;

public:
    thread_pool_run_pending();

    template<typename Func>
    auto submit(Func func);

    void run_pending();

    ~thread_pool_run_pending();

private:
    void run();

private:
    std::atomic_bool done;
    threadsafe_queue<task_type> queue;
    std::vector<std::jthread> threads;
};

thread_pool_run_pending::thread_pool_run_pending()
    : done{ false }
{
    for (auto hc = std::thread::hardware_concurrency(); hc--; )
    {
        threads.emplace_back(&thread_pool_run_pending::run, this);
    }
}

template<typename Func>
auto thread_pool_run_pending::submit(Func func)
{
    using ret_type = decltype(func());

    std::packaged_task<ret_type()> task{ std::move(func) };
    auto f = task.get_future();

    queue.push(std::move(task));

    return f;
}

void thread_pool_run_pending::run_pending()
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

void thread_pool_run_pending::run()
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

thread_pool_run_pending::~thread_pool_run_pending()
{
    done.store(true);
}

}
