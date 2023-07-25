#include "thread_pool_local_queue.hpp"

namespace tps
{

thread_local std::unique_ptr<std::queue<task>> thread_pool_local_queue::local_queue;

thread_pool_local_queue::thread_pool_local_queue()
    : done{ false }
{
    for (auto hc = std::thread::hardware_concurrency(); hc--; )
    {
        threads.emplace_back(&thread_pool_local_queue::run, this);
    }
}


void thread_pool_local_queue::run()
{
    local_queue = std::make_unique<std::queue<task>>();

    while (!done.load())
    {
        run_pending();
    }
}

void thread_pool_local_queue::run_pending()
{
    task task;

    if (local_queue != nullptr && !local_queue->empty())
    {
        task = std::move(local_queue->front());
        local_queue->pop();

        task();
    }
    else if (queue.try_get(task))
    {
        task();
    }
    else
    {
        std::this_thread::yield();
    }
}

thread_pool_local_queue::~thread_pool_local_queue()
{
    done.store(true);
}

}