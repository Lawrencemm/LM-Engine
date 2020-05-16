#pragma once

#include "input_event.h"

namespace lmtk
{
inline void click(iwidget &widget, lm::point2i point, input_state &input_state)
{
    input_state.mouse_state[lmpl::mouse_button::L] = true;
    widget.handle(
      mouse_button_down_event{input_state, lmpl::mouse_button::L, point});

    input_state.mouse_state[lmpl::mouse_button::L] = false;
    widget.handle(
      mouse_button_up_event{input_state, lmpl::mouse_button::L, point});
}

inline void hit(iwidget &widget, lmpl::key_code key, input_state &input_state)
{
    input_state.key_state[key] = true;
    widget.handle(key_down_event{input_state, key});
    input_state.key_state[key] = false;
    widget.handle(key_up_event{input_state, key});
}

inline void press(iwidget &widget, lmpl::key_code key, input_state &input_state)
{
    input_state.key_state[key] = true;
    widget.handle(key_down_event{input_state, key});
}

inline void lift(iwidget &widget, lmpl::key_code key, input_state &input_state)
{
    input_state.key_state[key] = false;
    widget.handle(key_up_event{input_state, key});
}

inline void
  move_mouse(iwidget &widget, lm::point2i point, input_state &input_state)
{
    lm::size2i delta;
    if (input_state.mouse_pos)
    {
        delta = point - input_state.mouse_pos.value();
    }
    input_state.mouse_pos = point;
    widget.handle(mouse_move_event{input_state, point});
}
} // namespace lmtk
