#pragma once

#include "event.h"
#include "resource_cache.h"
#include <entt/signal/sigh.hpp>
#include <lmlib/reference.h>

namespace lmtk
{
struct component_state
{
    std::optional<float> request_draw_in;
};
class component_interface
{
  public:
    virtual lmtk::component_state handle(lmtk::event const &event) = 0;

    virtual lm::size2i get_size() = 0;
    virtual lm::point2i get_position() = 0;

    virtual component_interface &
      set_rect(lm::point2i position, lm::size2i size) = 0;

    virtual component_interface &
      move_resources(lmgl::resource_sink &resource_sink) = 0;

    virtual ~component_interface() = default;
};

using component = lm::reference<component_interface>;

template <typename function_type, typename... arg_types>
bool collect_dirty_signal(entt::sigh<function_type> sigh, arg_types &&... args)
{
    bool is_someone_dirty{false};
    sigh.collect(
      [&is_someone_dirty](bool handler_return) {
          is_someone_dirty = is_someone_dirty || handler_return;
      },
      std::forward<arg_types>(args)...);

    return is_someone_dirty;
}
} // namespace lmtk
