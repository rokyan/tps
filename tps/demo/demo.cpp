#include "demo.hpp"
#include "thread_pool_simple.hpp"
#include "thread_pool_waitable_task.hpp"
#include "thread_pool_run_pending.hpp"
#include "thread_pool_local_queue.hpp"
#include "thread_pool_work_stealing.hpp"
#include <iostream>
#include <string>

namespace tps
{

void run_demo_simple()
{
    std::cout << "--- Demo simple ---" << std::endl;

    tps::thread_pool_simple thread_pool;

    thread_pool.submit([] {
        std::cout << "Hello from thread pool!\n";
    });

    std::this_thread::sleep_for(std::chrono::seconds(3));
}

void run_demo_waitable_task()
{
    std::cout << "--- Demo waitable task ---" << std::endl;

    tps::thread_pool_waitable_task thread_pool;

    auto twoxtwo = thread_pool.submit([] {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return 2 * 2;
    });

    std::cout << twoxtwo.get() << std::endl;
}

void run_demo_run_pending()
{
    std::cout << "--- Demo run pending ---" << std::endl;

    tps::thread_pool_run_pending thread_pool;

    for (auto tid = 0U; tid < std::thread::hardware_concurrency(); tid++)
    {
        thread_pool.submit([tid] {
            std::this_thread::sleep_for(std::chrono::seconds(2 + tid));
            std::cout << "Hello from thread pool!\n";
        });
    }

    thread_pool.submit([] {
        std::cout << "One more task\n";
    });

    thread_pool.run_pending();

    std::this_thread::sleep_for(std::chrono::seconds(10));
}

void run_demo_local_queue()
{
    std::cout << "--- Demo local queue ---" << std::endl;

    tps::thread_pool_local_queue thread_pool;

    for (auto tid = 0U; tid < std::thread::hardware_concurrency(); tid++)
    {
        thread_pool.submit([tid, &thread_pool] {
            std::this_thread::sleep_for(std::chrono::seconds(tid));
            const std::string message = "Hello from " + std::to_string(tid) + "!\n";
            std::cout << message;
            thread_pool.submit([tid] {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                const std::string message = "Hello again from " + std::to_string(tid) + "!\n";
                std::cout << message;
            });
        });
    }

    std::this_thread::sleep_for(std::chrono::seconds(8));
}

void run_demo_work_stealing()
{
    std::cout << "--- Demo work stealing ---" << std::endl;

    tps::thread_pool_work_stealing thread_pool;

    for (auto tid = 0U; tid < std::thread::hardware_concurrency(); tid++)
    {
        thread_pool.submit([tid, &thread_pool] {
            std::this_thread::sleep_for(std::chrono::seconds(tid));
            const std::string message = "Hello from " + std::to_string(tid) + "!\n";
            std::cout << message;
            thread_pool.submit([tid] {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                const std::string message = "Hello again from " + std::to_string(tid) + "!\n";
                std::cout << message;
            });
        });
    }

    std::this_thread::sleep_for(std::chrono::seconds(8));
}

void demo::run_demos()
{
    run_demo_simple();
    run_demo_waitable_task();
    run_demo_run_pending();
    run_demo_local_queue();
    run_demo_work_stealing();
}

}