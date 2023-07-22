#pragma once

#include "move_only_task.hpp"

#include <future>

namespace tps
{

class thread_pool_waitable_task final
{
private:
    using task_type = move_only_task;

public:
    thread_pool_waitable_task();

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

thread_pool_waitable_task::thread_pool_waitable_task()
    : done{ false }
{
    for (auto hc = std::thread::hardware_concurrency(); hc--; )
    {
        threads.emplace_back(&thread_pool_waitable_task::run, this);
    }
}

template<typename Func>
auto thread_pool_waitable_task::submit(Func func)
{
    using ret_type = decltype(func());

    std::packaged_task<ret_type()> task{ std::move(func) };
    auto f = task.get_future();

    queue.push(std::move(task));

    return f;
}

void thread_pool_waitable_task::run()
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

thread_pool_waitable_task::~thread_pool_waitable_task()
{
    done.store(true);
}

}
