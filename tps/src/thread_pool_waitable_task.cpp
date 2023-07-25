#include "thread_pool_waitable_task.hpp"

namespace tps
{

thread_pool_waitable_task::thread_pool_waitable_task()
    : done{ false }
{
    for (auto hc = std::thread::hardware_concurrency(); hc--; )
    {
        threads.emplace_back(&thread_pool_waitable_task::run, this);
    }
}

void thread_pool_waitable_task::run()
{
    while (!done.load())
    {
        task task;

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