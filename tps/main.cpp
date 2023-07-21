#include "thread_pool.hpp"
#include <iostream>
#include <thread>

int main()
{
    tps::thread_pool thread_pool;

    thread_pool.submit([] {
        std::cout << "Hello from thread pool!\n";
    });

    std::this_thread::sleep_for(std::chrono::seconds(3));

    return 0;
}