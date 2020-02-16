#include <lmlib/remote_executor.h>

lm::remote_executor::remote_executor(std::function<void()> func)
{
    stl_thread = std::thread{&remote_executor::body, this, func};
    ready_promise.get_future().get();
}

std::function<void()> lm::remote_executor::wait_pop_front()
{
    std::unique_lock<std::mutex> l{mutex};
    cv.wait(l, [this]() { return !queue.empty(); });
    auto ret = std::move(queue.front());
    queue.pop();
    return ret;
}

void lm::remote_executor::body(std::function<void()> func)
{
    func();
    ready_promise.set_value();
    do
    {
        wait_pop_front()();
    } while (!quit);
}
