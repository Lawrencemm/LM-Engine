#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>

namespace lm
{

class remote_executor
{
    typedef std::queue<std::function<void()>> Super;

  public:
    remote_executor(remote_executor &) = delete;
    remote_executor(std::function<void()> func = []() { ; });
    ~remote_executor()
    {
        run([this] { quit = true; });
        stl_thread.join();
    }

    template <typename Func> auto run(Func &&func) -> decltype(func());

  private:
    void body(std::function<void()> func);

  private:
    bool quit{false};
    std::thread stl_thread;
    std::queue<std::function<void()>> queue;
    std::promise<void> ready_promise;
    std::mutex mutex;
    std::condition_variable cv;

    std::function<void()> wait_pop_front();

    template <typename... Args> void protected_emplace(Args &&... args);
};

template <typename Func>
auto remote_executor::run(Func &&func) -> decltype(func())
{
    using FuncReturn = decltype(func());

    bool constexpr returning_void = std::is_same<FuncReturn, void>::value;

    if (std::this_thread::get_id() == stl_thread.get_id())
    {
        if constexpr (returning_void)
            func();
        else
            return func();
    }
    else
    {
        if constexpr (returning_void)
        {
            std::promise<void> promise;
            protected_emplace([&func, &promise]() {
                func();
                promise.set_value();
            });
            promise.get_future().get();
        }
        else
        {
            typedef decltype(func()) Promised;
            std::promise<Promised> promise;
            protected_emplace(
              [&func, &promise]() { promise.set_value(func()); });
            return promise.get_future().get();
        }
    }
}

template <typename... Args>
void remote_executor::protected_emplace(Args &&... args)
{
    std::lock_guard<std::mutex> l{mutex};
    queue.emplace(std::forward<Args>(args)...);
    cv.notify_all();
}
} // namespace lm
