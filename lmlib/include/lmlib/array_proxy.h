#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <vector>

namespace lm
{
template <typename element_type> class array_proxy
{
  public:
    explicit array_proxy(std::nullptr_t)
        : n_elements(0), first_element_addr(nullptr)
    {
    }

    explicit array_proxy(element_type &ptr)
        : n_elements(1), first_element_addr(&ptr)
    {
    }

    array_proxy(element_type *ptr, size_t count)
        : n_elements(count), first_element_addr(ptr)
    {
    }

    template <size_t array_size>
    array_proxy(
      std::array<typename std::remove_const<element_type>::type, array_size>
        &data)
        : n_elements(array_size), first_element_addr(data.data())
    {
    }

    template <size_t array_size>
    array_proxy(std::array<
                typename std::remove_const<element_type>::type,
                array_size> const &data)
        : n_elements(array_size), first_element_addr(data.data())
    {
    }

    template <
      class allocator_type =
        std::allocator<typename std::remove_const<element_type>::type>>
    array_proxy(std::vector<
                typename std::remove_const<element_type>::type,
                allocator_type> &data)
        : n_elements(static_cast<size_t>(data.size())),
          first_element_addr(data.data())
    {
    }

    template <
      class allocator_type =
        std::allocator<typename std::remove_const<element_type>::type>>
    array_proxy(std::vector<
                typename std::remove_const<element_type>::type,
                allocator_type> const &data)
        : n_elements(static_cast<size_t>(data.size())),
          first_element_addr(data.data())
    {
    }

    array_proxy(std::initializer_list<element_type> const &data)
        : n_elements(static_cast<size_t>(data.end() - data.begin())),
          first_element_addr(data.begin())
    {
    }

    const element_type *begin() const { return first_element_addr; }

    const element_type *end() const { return first_element_addr + n_elements; }

    const element_type &front() const
    {
        assert(n_elements && first_element_addr);
        return *first_element_addr;
    }

    const element_type &back() const
    {
        assert(n_elements && first_element_addr);
        return *(first_element_addr + n_elements - 1);
    }

    bool empty() const { return (n_elements == 0); }

    size_t size() const { return n_elements; }

    element_type *data() const { return first_element_addr; }

    const element_type &operator[](size_t index) const
    {
        return *(first_element_addr + index);
    }

    size_t n_elements;
    element_type *const first_element_addr;
};

template <typename container_type>
array_proxy<char const> raw_array_proxy(const container_type &container)
{
    return array_proxy{(char const *)container.data(),
                       container.size() * sizeof(*container.data())};
};

template <typename F, typename S>
array_proxy<char const> raw_array_proxy(const std::pair<F, S> &container)
{
    return array_proxy{(char const *)&container, sizeof(F) + sizeof(S)};
}
} // namespace lm
