#pragma once
#include <array>
#include <cmath>

namespace lm
{
template <typename Integral> class range
{
  public:
    explicit constexpr range(Integral endValue) : start(0), endValue(endValue) {}
    range(Integral start, Integral endValue) : start(start), endValue(endValue)
    {
    }

    class range_iterator
    {
      public:
        using difference_type = Integral;
        using iterator_category = std::input_iterator_tag;
        using value_type = Integral;
        using pointer = Integral*;
        using reference = Integral&;

        friend range;

        range_iterator &operator++()
        {
            value += 1;
            return *this;
        }

        range_iterator operator++(int)
        {
            value += 1;
            return range_iterator(value - 1, end);
        }

        difference_type operator-(const range_iterator& other) const
        {
            return value - other.value;
        }

        bool operator==(const range_iterator &other) const
        {
            return (value == other.value && end == other.end) ||
                   (ended() && other.ended());
        }
        bool operator!=(const range_iterator &other) const
        {
            return !(*this == other);
        }
        Integral & operator*() { return value; }

      private:
        range_iterator(Integral end) : value(end), end(end) {}
        range_iterator(Integral value, Integral end) : value(value), end(end) {}
        bool ended() const { return value >= end; }

        Integral value;
        Integral end;
    };

    range_iterator begin() const { return range_iterator(start, endValue); }
    range_iterator end() const { return range_iterator(endValue); }

  private:
    Integral start;
    Integral endValue;
};
} // namespace lm
