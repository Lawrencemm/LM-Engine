#include "../entity_editor_controller.h"
#include <lmlib/math_constants.h>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/concat.hpp>

namespace lmeditor
{
template <int axis, int sign>
entity_editor_controller::command create_rotate_command(char const *name)
{
    return entity_editor_controller::command{
      [](entity_editor_controller::command_args const &args) {
          float amount{0.1f * lm::pi};

          if (args.key_down_event.input_state.key_state.shift())
              amount = 0.5f * lm::pi;

          amount *= sign;

          Eigen::Vector3f axis_vector{0.f, 0.f, 0.f};
          axis_vector[axis] = 1.f;

          auto &transform =
            args.map.get<lmng::transform>(args.controller.get_selection());

          transform.rotation =
            transform.rotation * Eigen::AngleAxisf{amount, axis_vector};

          return true;
      },
      name,
    };
}

entity_editor_controller::command quit_rotate_command{
  [](entity_editor_controller::command_args const &args) {
      args.controller.leave_state();
      return true;
  },
  "Stop rotating entity",
};

entity_editor_controller::command_list rotate_commands{
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

entity_editor_controller::rotate_state::rotate_state(
  entity_editor_controller::select_state &,
  entity_editor_controller &)
    : commands{ranges::views::concat(entity_editor_controller::viewport_commands, rotate_commands) | ranges::to<command_list>()},
      key_command_map{ranges::views::all(commands) | ranges::to<command_map>()}
{
}
} // namespace lmeditor
