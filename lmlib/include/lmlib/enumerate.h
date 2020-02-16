#pragma once

#include <iterator>
#include <tuple>

namespace lm
{

template <class iterator_type> struct enumerate_iterator
{
    using iterator = iterator_type;
    using index_type = typename std::iterator_traits<iterator>::difference_type;
    using reference = typename std::iterator_traits<iterator>::reference;

    enumerate_iterator(index_type index, iterator iterator)
        : index(index), iter(iterator)
    {
    }

    enumerate_iterator &operator++()
    {
        ++index;
        ++iter;
        return *this;
    }

    bool operator!=(const enumerate_iterator &other) const
    {
        return iter != other.iter;
    }

    std::pair<index_type &, reference> operator*() { return {index, *iter}; }
    std::pair<const index_type &, const reference> operator*() const
    {
        return {index, *iter};
    }

  private:
    index_type index;
    iterator iter;
};

template <class iterator_type> struct enumerate_range
{
    using index_type =
      typename std::iterator_traits<iterator_type>::difference_type;
    using iterator = enumerate_iterator<iterator_type>;

    enumerate_range(iterator_type first, iterator_type last, index_type initial)
        : first(first), last(last), initial(initial)
    {
    }

    iterator begin() const { return iterator(initial, first); }

    iterator end() const { return iterator(0, last); }

  private:
    iterator_type first;
    iterator_type last;
    index_type initial;
};

template <class iterator>
decltype(auto) enumerate(
  iterator first,
  iterator last,
  typename std::iterator_traits<iterator>::difference_type initial = 0)
{
    return enumerate_range(first, last, initial);
}

template <class container_type>
decltype(auto) enumerate(const container_type &container)
{
    return enumerate_range(std::begin(container), std::end(container), 0);
}
} // namespace lm