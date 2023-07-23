#pragma once

#include "threadsafe_queue.hpp"
#include "move_only_task.hpp"
#include <type_traits>
#include <future>
#include <atomic>
#include <memory>
#include <queue>

namespace tps
{

class thread_pool_local_queue final
{
private:
    using task_type = move_only_task;

public:
    thread_pool_local_queue();

    template<typename Func>
    auto submit(Func func)
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

    void run_pending();

    ~thread_pool_local_queue();

private:
    void run();

private:
    std::atomic_bool done;
    threadsafe_queue<task_type> queue;
    std::vector<std::jthread> threads;
    static thread_local std::unique_ptr<std::queue<task_type>> local_queue;
};

}
