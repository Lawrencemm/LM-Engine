#pragma once

#include <chrono>

namespace lm
{
class realtime_clock
{
    using clock_type = std::chrono::high_resolution_clock;
    clock_type::time_point simulation_time{clock_type::now()};

  public:
    float tick()
    {

        auto now = clock_type::now();
        std::chrono::duration<float> dt = now - simulation_time;
        simulation_time = now;
        return dt.count();
    }
};
} // namespace lm
