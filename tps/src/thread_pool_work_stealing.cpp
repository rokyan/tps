#include "thread_pool_work_stealing.hpp"

namespace tps
{

thread_local work_stealing_queue<thread_pool_work_stealing::task_type>* thread_pool_work_stealing::local_queue;
thread_local std::size_t thread_pool_work_stealing::id;

thread_pool_work_stealing::thread_pool_work_stealing()
    : done{ false }
{
    auto hc = std::thread::hardware_concurrency();

    for (auto id = 0U; id < hc; id++)
    {
        local_queues.push_back(std::make_unique<work_stealing_queue<task_type>>());
    }

    for (auto id = 0U; id < hc; id++)
    {
        threads.emplace_back(&thread_pool_work_stealing::run, this, id);
    }
}

void thread_pool_work_stealing::run(std::size_t id)
{
    this->id = id;
    local_queue = local_queues[id].get();

    while (!done.load())
    {
        run_pending();
    }
}

void thread_pool_work_stealing::run_pending()
{
    task_type task;

    if (pop_from_local_queue(task) ||
        pop_from_other_local_queue(task) ||
        pop_from_queue(task))
    {
        task();
    }
    else
    {
        std::this_thread::yield();
    }
}

bool thread_pool_work_stealing::pop_from_local_queue(task_type& task)
{
    return local_queue != nullptr && local_queue->try_pop(task);
}

bool thread_pool_work_stealing::pop_from_queue(task_type& task)
{
    return queue.try_get(task);
}

bool thread_pool_work_stealing::pop_from_other_local_queue(task_type& task)
{
    for (auto idx = 0U; idx < std::size(local_queues); idx++)
    {
        const auto next_idx = (idx + id + 1) % std::size(local_queues);

        if (local_queues[next_idx]->try_steal(task))
        {
            return true;
        }
    }

    return false;
}

thread_pool_work_stealing::~thread_pool_work_stealing()
{
    done.store(true);
}

}