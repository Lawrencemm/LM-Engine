#pragma once

#include <thread>

#include <functional>
#include <iostream>
#include <tbb/concurrent_queue.h>

namespace lm
{
class worker_thread
{
  public:
    worker_thread()
        : queue{}, stl_thread{[this] {
              while (!quit)
              {
                  std::function<void()> fun;
                  queue.pop(fun);
                  fun();
              }
          }}
    {
    }
    ~worker_thread()
    {
        queue.emplace([this] { quit = true; });
        stl_thread.join();
    }
    void job(std::function<void()> fun) { queue.emplace(fun); }

  private:
    bool quit{false};
    tbb::concurrent_bounded_queue<std::function<void()>> queue;
    std::thread stl_thread;
};

class loop_worker_thread
{
  public:
    explicit loop_worker_thread(
      const std::function<void()> &body,
      const std::function<void()> &interrupt = std::function<void()>{})
        : interrupt{interrupt},
          stl_thread{
            [this, body] {
                while (!quit)
                {
                    body();
                }
            },
          }
    {
    }
    ~loop_worker_thread()
    {
        quit = true;
        if (interrupt)
            interrupt();
        stl_thread.join();
    }

  private:
    bool quit{false};
    std::function<void()> interrupt;
    std::thread stl_thread;
};
} // namespace lm
