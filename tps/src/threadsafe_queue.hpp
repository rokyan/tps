#pragma once

#include <utility>
#include <queue>
#include <mutex>

namespace tps
{

template<typename T>
class threadsafe_queue final
{
public:
    threadsafe_queue() = default;

    threadsafe_queue(const threadsafe_queue&) = delete;
    threadsafe_queue& operator=(const threadsafe_queue&) = delete;

    threadsafe_queue(threadsafe_queue&&) = default;
    threadsafe_queue& operator=(threadsafe_queue&&) = default;

    void push(const T& vale);
    void push(T&& value);

    bool try_get(T& value);

private:
    std::queue<T> data;
    std::mutex data_mutex;
};

template<typename T>
void threadsafe_queue<T>::push(const T& value)
{
    std::lock_guard lock(data_mutex);
    data.push(value);
}

template<typename T>
void threadsafe_queue<T>::push(T&& value)
{
    std::lock_guard lock(data_mutex);
    data.push(std::move(value));
}

template<typename T>
bool threadsafe_queue<T>::try_get(T& value)
{
    std::unique_lock lock(data_mutex, std::defer_lock);

    if (lock.try_lock() && !data.empty())
    {
        value = std::move(data.front());
        data.pop();

        return true;
    }

    return false;
}

}
