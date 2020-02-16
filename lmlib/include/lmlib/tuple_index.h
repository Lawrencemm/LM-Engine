#pragma once

#include <cstddef>
#include <type_traits>

namespace lm
{
template <class element_type, class tuple_type> struct tuple_index
{
    static const std::size_t value = 0;
    static_assert(
      !std::is_same_v<tuple_type, std::tuple<>>,
      "Could not find type in given tuple.");
};

template <class element_type, class... tail_types>
struct tuple_index<element_type, std::tuple<element_type, tail_types...>>
{
    static const std::size_t value = 0;
};

template <class element_type, class head_type, class... tail_types>
struct tuple_index<element_type, std::tuple<head_type, tail_types...>>
{
    static const std::size_t value =
      1 + tuple_index<element_type, std::tuple<tail_types...>>::value;
};
}