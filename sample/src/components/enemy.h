#pragma once

#include <ostream>

struct enemy_component
{
};

inline std::ostream &operator<<(std::ostream &os, enemy_component const &)
{
    return os;
}
