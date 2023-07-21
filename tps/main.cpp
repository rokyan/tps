#include "thread_pool_simple.hpp"
#include "thread_pool_waitable_task.hpp"

#include <iostream>
#include <thread>

void demo_thread_pool_simple()
{
    tps::thread_pool_simple thread_pool;

    thread_pool.submit([] {
        std::cout << "Hello from thread pool!\n";
    });

    std::this_thread::sleep_for(std::chrono::seconds(3));
}

void demo_thread_pool_waitable_task()
{
    tps::thread_pool_waitable_task thread_pool;

    auto f = thread_pool.submit([] {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return 2 * 2;
    });

    std::cout << f.get() << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(3));
}

int main()
{
    demo_thread_pool_simple();
    demo_thread_pool_waitable_task();

    return 0;
}