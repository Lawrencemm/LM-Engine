#pragma once

#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>

#include <tbb/concurrent_queue.h>

namespace lm
{

class exception : public std::exception
{
  public:
    exception(const char *msg) : msg{msg} {}

    virtual const char *what() const noexcept override { return msg; }

  private:
    const char *msg;
};

} // namespace lm

#ifndef NDEBUG
#define DEBUG_OUTPUT(obj) std::cerr << obj
#define DEBUG_OUTPUT_LINE(obj) std::cerr << obj << std::endl
#else
#define DEBUG_OUTPUT(str)
#define DEBUG_OUTPUT_LINE(str)
#endif