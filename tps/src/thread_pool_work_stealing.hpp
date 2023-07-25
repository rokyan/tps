#pragma once

#include "threadsafe_queue.hpp"
#include "work_stealing_queue.hpp"
#include "move_only_task.hpp"
#include <thread>
#include <future>
#include <atomic>
#include <memory>
#include <queue>

namespace tps
{

class thread_pool_work_stealing final
{
private:
    using task_type = move_only_task;

public:
    thread_pool_work_stealing();

    thread_pool_work_stealing(const thread_pool_work_stealing&) = delete;
    thread_pool_work_stealing& operator=(const thread_pool_work_stealing&) = delete;

    thread_pool_work_stealing(thread_pool_work_stealing&&) = default;
    thread_pool_work_stealing& operator=(thread_pool_work_stealing&&) = default;

    template<typename Func>
    auto submit(Func func);

    void run_pending();

    ~thread_pool_work_stealing();

private:
    void run(std::size_t id);

    bool pop_from_local_queue(task_type& task);
    bool pop_from_queue(task_type& task);
    bool pop_from_other_local_queue(task_type& task);

private:
    std::atomic_bool done;
    threadsafe_queue<task_type> queue;
    std::vector<std::unique_ptr<work_stealing_queue<task_type>>> local_queues;
    std::vector<std::jthread> threads;
    static thread_local work_stealing_queue<task_type>* local_queue;
    static thread_local std::size_t id;
};

template<typename Func>
auto thread_pool_work_stealing::submit(Func func)
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
