#pragma once

#include "input_state.h"
#include "lmgl/renderer.h"
#include "lmgl/resource_sink.h"
#include "resource_cache.h"
#include <lmlib/geometry.h>
#include <lmpl/lmpl.h>
#include <optional>
#include <variant>

namespace lmtk
{
struct key_down_event
{
    const struct input_state &input_state;
    lmpl::key_code key;
};
struct key_up_event
{
    const struct input_state &input_state;
    lmpl::key_code key;
};
struct mouse_button_down_event
{
    const struct input_state &input_state;
    lmpl::mouse_button button;
    lm::point2i pos;
};
struct mouse_button_up_event
{
    const struct input_state &input_state;
    lmpl::mouse_button button;
    lm::point2i pos;
};
struct mouse_move_event
{
    const struct input_state &input_state;
    lm::point2i pos;
    lm::size2i delta;
};
struct resize_event
{
    lmpl::iwindow &window;
};
struct draw_event
{
    lmgl::irenderer &renderer;
    lmgl::iframe &frame;
    lmgl::resource_sink &resource_sink;
    lmtk::resource_cache const &resource_cache;
    lmtk::input_state const &input_state;
};
struct quit_event
{
};
using event = std::variant<
  key_down_event,
  key_up_event,
  mouse_button_down_event,
  mouse_button_up_event,
  mouse_move_event,
  resize_event,
  draw_event,
  quit_event>;

/// Create input event from platform window event, updating the input state.
std::optional<event>
  create_input_event(const lmpl::window_message &msg, input_state &input_state);
} // namespace lmtk
