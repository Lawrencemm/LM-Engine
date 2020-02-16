#pragma once

#include <type_traits>
#include <variant>

namespace lm
{
template <class... Ts> struct variant_visitor : Ts...
{
    using Ts::operator()...;
};
template <class... Ts> variant_visitor(Ts...)->variant_visitor<Ts...>;

template <typename... Types, typename... Visitors>
auto operator>>(
  std::variant<Types...> &variant, const variant_visitor<Visitors...> &visitor)
{
    return std::visit(visitor, variant);
}

template <typename... Types, typename... Visitors>
auto operator>>(
  const std::variant<Types...> &variant,
  const variant_visitor<Visitors...> &visitor)
{
    return std::visit(visitor, variant);
}
} // namespace lm
