#pragma once

#include <cstddef>

namespace lm
{
template <typename container_type>
size_t array_size(const container_type &container)
{
    return sizeof(container[0]) * container.size();
}
} // namespace lm
