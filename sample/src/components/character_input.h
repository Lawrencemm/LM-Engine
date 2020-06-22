#pragma once

#include <ostream>

struct character_input
{
    float radians_per_dot;
};

inline std::ostream &operator<<(std::ostream &os, character_input const &)
{
    return os;
}
