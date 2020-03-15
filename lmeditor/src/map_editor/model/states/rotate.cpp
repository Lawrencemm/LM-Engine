#include "../model.h"
#include <range/v3/view/concat.hpp>

#include <lmlib/math_constants.h>

namespace lmeditor
{
template <int axis, int sign>
map_editor_model::command create_rotate_command(char const *name)
{
    return map_editor_model::command{
      [](map_editor_model::command_args const &args) {
          float amount{0.1f * lm::pi};

          if (args.key_down_event.input_state.key_state.shift())
              amount = 0.5f * lm::pi;

          amount *= sign;

          Eigen::Vector3f axis_vector{0.f, 0.f, 0.f};
          axis_vector[axis] = 1.f;

          auto &transform =
            args.map.get<lmng::transform>(args.model.selected_box);

          transform.rotation =
            transform.rotation * Eigen::AngleAxisf{amount, axis_vector};

          args.event_handler(map_editor_modified_selected{args.map});
          return true;
      },
      name,
    };
}

map_editor_model::command quit_rotate_command{
  [](map_editor_model::command_args const &args) {
      args.model.leave_state(
        *static_cast<map_editor_model::rotate_state *>(args.state_ptr),
        args.on_state_change);
      return true;
  },
  "Stop rotating entity",
};

map_editor_model::command_list rotate_commands{
  {{lmpl::key_code::K},
   create_rotate_command<0, 1>("Rotate around x axis (positive)")},
  {{lmpl::key_code::I},
   create_rotate_command<0, -1>("Rotate around x axis (negative)")},
  {{lmpl::key_code::L},
   create_rotate_command<1, 1>("Rotate around y axis (positive)")},
  {{lmpl::key_code::J},
   create_rotate_command<1, -1>("Rotate around y axis (negative)")},
  {{lmpl::key_code::U},
   create_rotate_command<2, 1>("Rotate around z axis (positive)")},
  {{lmpl::key_code::O},
   create_rotate_command<2, -1>("Rotate around z axis (negative)")},
  {{lmpl::key_code::Q}, quit_rotate_command},
};

map_editor_model::rotate_state::rotate_state(
  map_editor_model::select_state &,
  map_editor_model &)
    : commands{ranges::views::concat(
                 map_editor_model::viewport_commands,
                 rotate_commands) |
               ranges::to<command_list>()},
      key_command_map{ranges::views::all(commands) | ranges::to<command_map>()}
{
}
} // namespace lmeditor
