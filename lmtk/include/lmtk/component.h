#pragma once

#include "input_event.h"
#include "resource_cache.h"
#include "widget.h"
#include <entt/signal/sigh.hpp>
#include <lmlib/reference.h>
#include <any>

namespace lmtk
{
class component_interface : public widget_interface
{
  public:
    virtual bool handle(const lmtk::input_event &input_event, std::any model) = 0;
    virtual component_interface &update(
      lmgl::irenderer *renderer,
      lmgl::resource_sink &resource_sink,
      lmtk::resource_cache const &resource_cache,
      lmtk::input_state const &input_state,
      std::any model) = 0;
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
