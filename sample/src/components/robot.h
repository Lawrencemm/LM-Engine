#pragma once

struct robot
{
};

inline std::ostream &operator<<(std::ostream &os, robot const &) { return os; }
