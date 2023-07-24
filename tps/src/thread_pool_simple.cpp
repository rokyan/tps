#include "thread_pool_simple.hpp"

namespace tps
{

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