#pragma once

#include <optional>
#include <tuple>

#include "variant_visitor.h"

namespace lm
{

/// CRTP class to mixin a state machine that uses a variant for dispatching to
/// different states.
template <
  typename deriving_type,
  typename dispatch_return_type,
  typename... state_types>
class internal_fsm
{
  public:
    using state_variant_type = std::variant<state_types...>;
    using state_type_optional = std::optional<state_variant_type>;

    state_variant_type state;

  public:
    template <typename event_type>
    dispatch_return_type dispatch(event_type &&event)
    {
        auto deriving_type_this = static_cast<deriving_type *>(this);
        auto ret = state >> variant_visitor{[&](auto &state) {
                       return deriving_type_this->handle(
                         state, std::forward<event_type>(event));
                   }};

        auto new_state_variant = std::get<0>(ret);
        if (new_state_variant)
        {
            state = std::move(new_state_variant.value());
        }
        return std::get<1>(ret);
    }

    template <typename... event_variant_type>
    dispatch_return_type
      dispatch_variant(const std::variant<event_variant_type...> &event)
    {
        auto deriving_type_this = static_cast<deriving_type *>(this);
        auto ret = std::visit(
          variant_visitor{[&](auto &state, auto &event) {
              return deriving_type_this->handle(state, event);
          }},
          state,
          event);

        auto new_state_variant = std::move(std::get<0>(ret));
        if (new_state_variant)
        {
            state = std::move(new_state_variant.value());
        }
        return std::get<1>(ret);
    }
};
} // namespace lm
