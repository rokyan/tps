#include "thread_pool_run_pending.hpp"

namespace tps
{

thread_pool_run_pending::thread_pool_run_pending()
    : done{ false }
{
    for (auto hc = std::thread::hardware_concurrency(); hc--; )
    {
        threads.emplace_back(&thread_pool_run_pending::run, this);
    }
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