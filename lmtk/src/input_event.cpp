#include <lmlib/variant_visitor.h>
#include <lmtk/input_event.h>

namespace lmtk
{
std::optional<input_event>
  create_input_event(const lmpl::window_message &msg, input_state &input_state)
{
    using optev = std::optional<input_event>;
    return msg >>
           lm::variant_visitor{
             [&](const lmpl::key_down_message &msg) -> optev {
                 input_state.key_state.set(msg.key, true);
                 return key_down_event{input_state, msg.key};
             },
             [&](const lmpl::key_up_message &msg) -> optev {
                 input_state.key_state.set(msg.key, false);
                 return key_up_event{input_state, msg.key};
             },
             [&](const lmpl::mouse_button_down_message &msg) -> optev {
                 input_state.mouse_state[msg.button] = true;
                 return mouse_button_down_event{
                   input_state, msg.button, msg.pos};
             },
             [&](const lmpl::mouse_button_up_message &msg) -> optev {
                 input_state.mouse_state[msg.button] = false;
                 return mouse_button_up_event{input_state, msg.button, msg.pos};
             },
             [&](const lmpl::mouse_move_message &msg) -> optev {
                 lm::size2i delta;
                 if (input_state.mouse_pos)
                 {
                     delta = msg.pos - input_state.mouse_pos.value();
                 }
                 else
                 {
                     delta = {0, 0};
                 }
                 input_state.mouse_pos = msg.pos;
                 return mouse_move_event{input_state, msg.pos, delta};
             },
             [&](const lmpl::lose_focus_message &msg) -> optev {
                 input_state.reset();
                 return std::nullopt;
             },
             [](const auto &msg) -> optev { return std::nullopt; },
           };
}
} // namespace lmtk
