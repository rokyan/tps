#pragma once

#include <utility>
#include <memory>

namespace tps
{

class move_only_task final
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
    move_only_task() = default;

    template<typename Func>
    move_only_task(Func func)
        : func{ std::make_unique<impl<Func>>(std::move(func)) }
    {}

    move_only_task(const move_only_task&) = delete;
    move_only_task& operator=(const move_only_task&) = delete;

    move_only_task(move_only_task&&) = default;
    move_only_task& operator=(move_only_task&&) = default;

    void operator()()
    {
        func->operator()();
    }

private:
    std::unique_ptr<impl_base> func;
};

}
