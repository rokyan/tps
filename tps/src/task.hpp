#pragma once

#include <utility>
#include <memory>

namespace tps
{

class task final
{
private:
    class impl_base
    {
    public:
        virtual void operator()() = 0;
        virtual ~impl_base() = default;
    };

    template<typename Func>
    class impl : public impl_base
    {
    public:
        impl(Func func)
            : func{ std::move(func) }
        {}

        void operator()() override
        {
            func();
        }

    private:
        Func func;
    };

public:
    task() = default;

    template<typename Func>
    task(Func func)
        : func{ std::make_unique<impl<Func>>(std::move(func)) }
    {}

    task(const task&) = delete;
    task& operator=(const task&) = delete;

    task(task&&) = default;
    task& operator=(task&&) = default;

    void operator()()
    {
        func->operator()();
    }

private:
    std::unique_ptr<impl_base> func;
};

}
