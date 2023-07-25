#pragma once

#include <utility>
#include <deque>
#include <mutex>

namespace tps
{

template<typename T>
class work_stealing_queue final
{
public:
    work_stealing_queue() = default;

    work_stealing_queue(const work_stealing_queue&) = delete;
    work_stealing_queue& operator=(const work_stealing_queue&) = delete;

    work_stealing_queue(work_stealing_queue&&) = default;
    work_stealing_queue& operator=(work_stealing_queue&&) = default;

    void push(const T& value);
    void push(T&& value);

    bool try_pop(T& value);

    bool try_steal(T& value);

    bool empty() const;

private:
    std::deque<T> data;
    mutable std::mutex data_mutex;
};

template<typename T>
void work_stealing_queue<T>::push(const T& value)
{
    std::lock_guard lock(data_mutex);
    data.push_front(value);
}

template<typename T>
void work_stealing_queue<T>::push(T&& value)
{
    std::lock_guard lock(data_mutex);
    data.push_front(std::move(value));
}

template<typename T>
bool work_stealing_queue<T>::try_pop(T& value)
{
    std::unique_lock lock(data_mutex, std::defer_lock);

    if (lock.try_lock() && !data.empty())
    {
        value = std::move(data.front());
        data.pop_front();
        return true;
    }

    return false;
}

template<typename T>
bool work_stealing_queue<T>::try_steal(T& value)
{
    std::unique_lock lock(data_mutex, std::defer_lock);

    if (lock.try_lock() && !data.empty())
    {
        value = std::move(data.back());
        data.pop_back();
        return true;
    }

    return false;
}

template<typename T>
bool work_stealing_queue<T>::empty() const
{
    std::lock_guard lock(data_mutex);
    return data.empty();
}

}
