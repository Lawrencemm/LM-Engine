#pragma once
#include <assert.h>
#include <memory>

namespace lm
{
template <typename pointed_type>
class reference : public std::unique_ptr<pointed_type>
{
  public:
    using super = std::unique_ptr<pointed_type>;
    using super::super;
    reference(std::nullptr_t) = delete;
    reference(super &&other) : super{std::move(other)} {}
    reference(const super &other) : super{other} {}
    reference(pointed_type *ptr) = delete;
    pointed_type &operator*() const noexcept
    {
        assert(super::get());
        return *super::get();
    }
    pointed_type *operator->() const noexcept
    {
        assert(super::get());
        return super::get();
    }
    pointed_type *get() const noexcept
    {
        assert(super::get());
        return super::get();
    }

    //  reference &operator=(reference &other)
    //  {
    //    return super::operator=(other);
    //  }
};
} // namespace lm
