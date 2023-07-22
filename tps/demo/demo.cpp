#include "demo.hpp"
#include "thread_pool_simple.hpp"
#include "thread_pool_waitable_task.hpp"
#include "thread_pool_run_pending.hpp"
#include <iostream>

namespace tps
{

void run_demo_simple()
{
    tps::thread_pool_simple thread_pool;

    thread_pool.submit([] {
        std::cout << "Hello from thread pool!\n";
    });

    std::this_thread::sleep_for(std::chrono::seconds(3));
}

void run_demo_waitable_task()
{
    tps::thread_pool_waitable_task thread_pool;

    auto twoxtwo = thread_pool.submit([] {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return 2 * 2;
    });

    std::cout << twoxtwo.get() << std::endl;
}

void run_demo_run_pending()
{
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

void demo::run_demos()
{
    run_demo_simple();
    run_demo_waitable_task();
    run_demo_run_pending();
}

}