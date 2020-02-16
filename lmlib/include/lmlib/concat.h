#pragma once

#include "enumerate.h"
#include <array>
#include <iostream>

namespace lm
{
template <
  typename element_type, std::size_t lhs_array_size, std::size_t rhs_array_size>
std::array<element_type, lhs_array_size + rhs_array_size> concat(
  const std::array<element_type, lhs_array_size> &lhs,
  const std::array<element_type, rhs_array_size> &rhs)
{
    std::array<element_type, lhs_array_size + rhs_array_size> result{};
    std::size_t index = 0;

    for (auto &el : lhs)
    {
        result[index] = el;
        ++index;
    }
    for (auto &el : rhs)
    {
        result[index] = el;
        ++index;
    }

    return result;
}

template <typename element_type, std::size_t lhs_array_size>
std::array<element_type, lhs_array_size + 1> concat(
  const std::array<element_type, lhs_array_size> &lhs, const element_type &elem)
{
    std::array<element_type, lhs_array_size + 1> result{};

    for (auto const &[index, item] : enumerate(lhs))
    {
        result[index] = item;
    }

    result.back() = elem;

    return result;
}

} // namespace lm
