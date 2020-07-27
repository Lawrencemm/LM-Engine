#include "../visual_registry_controller.h"
#include <range/v3/to_container.hpp>
#include <range/v3/view/concat.hpp>

namespace lmeditor
{
void do_copy(
  visual_registry_controller::command_args const &args,
  Eigen::Vector3f const &direction)
{
    auto new_box = args.controller.copy_entity(
      args.map, args.controller.view_to_axis(direction));

    args.controller.select(new_box);

    args.controller.leave_state();
}

visual_registry_controller::command copy_above_command{
  [](visual_registry_controller::command_args const &args) {
      do_copy(args, Eigen::Vector3f::UnitY());
      return true;
  },
  "Add entity above",
};

visual_registry_controller::command copy_below_command{
  [](visual_registry_controller::command_args const &args) {
      do_copy(args, -Eigen::Vector3f::UnitY());
      return true;
  },
  "Add entity below",
};

visual_registry_controller::command copy_left_command{
  [](visual_registry_controller::command_args const &args) {
      do_copy(args, -Eigen::Vector3f::UnitX());
      return true;
  },
  "Add entity left",
};

visual_registry_controller::command copy_right_command{
  [](visual_registry_controller::command_args const &args) {
      do_copy(args, Eigen::Vector3f::UnitX());
      return true;
  },
  "Add entity right",
};

visual_registry_controller::command copy_behind_command{
  [](visual_registry_controller::command_args const &args) {
      do_copy(args, Eigen::Vector3f::UnitZ());
      return true;
  },
  "Add entity behind",
};

visual_registry_controller::command copy_front_command{
  [](visual_registry_controller::command_args const &args) {
      do_copy(args, -Eigen::Vector3f::UnitZ());
      return true;
  },
  "Add entity in front",
};

static visual_registry_controller::command cancel_command{
  [](visual_registry_controller::command_args const &args) {
      args.controller.leave_state();
      return true;
  },
  "Cancel entity add",
};

visual_registry_controller::command_list copy_adjacent_commands{
  {{lmpl::key_code::I}, copy_above_command},
  {{lmpl::key_code::K}, copy_below_command},
  {{lmpl::key_code::J}, copy_left_command},
  {{lmpl::key_code::L}, copy_right_command},
  {{lmpl::key_code::U}, copy_front_command},
  {{lmpl::key_code::O}, copy_behind_command},
  {{lmpl::key_code::Q}, cancel_command},
};

visual_registry_controller::copy_entity_state::copy_entity_state(
  visual_registry_controller::select_state &,
  visual_registry_controller &controller)
    : commands{ranges::views::concat(visual_registry_controller::viewport_commands, copy_adjacent_commands) | ranges::to<command_list>()},
      key_command_map{ranges::views::all(commands) | ranges::to<command_map>()}
{
}
} // namespace lmeditor
