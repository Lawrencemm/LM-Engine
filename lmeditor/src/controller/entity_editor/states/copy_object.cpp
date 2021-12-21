#include "../entity_editor_controller.h"
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/concat.hpp>

namespace lmeditor
{
void do_copy(
  entity_editor_controller::command_args const &args,
  Eigen::Vector3f const &direction)
{
    auto new_box = args.controller.copy_entity(
      args.map, args.controller.view_to_axis(direction));

    args.controller.select(new_box);

    args.controller.leave_state();
}

entity_editor_controller::command copy_above_command{
  [](entity_editor_controller::command_args const &args) {
      do_copy(args, Eigen::Vector3f::UnitY());
      return true;
  },
  "Add entity above",
};

entity_editor_controller::command copy_below_command{
  [](entity_editor_controller::command_args const &args) {
      do_copy(args, -Eigen::Vector3f::UnitY());
      return true;
  },
  "Add entity below",
};

entity_editor_controller::command copy_left_command{
  [](entity_editor_controller::command_args const &args) {
      do_copy(args, -Eigen::Vector3f::UnitX());
      return true;
  },
  "Add entity left",
};

entity_editor_controller::command copy_right_command{
  [](entity_editor_controller::command_args const &args) {
      do_copy(args, Eigen::Vector3f::UnitX());
      return true;
  },
  "Add entity right",
};

entity_editor_controller::command copy_behind_command{
  [](entity_editor_controller::command_args const &args) {
      do_copy(args, Eigen::Vector3f::UnitZ());
      return true;
  },
  "Add entity behind",
};

entity_editor_controller::command copy_front_command{
  [](entity_editor_controller::command_args const &args) {
      do_copy(args, -Eigen::Vector3f::UnitZ());
      return true;
  },
  "Add entity in front",
};

static entity_editor_controller::command cancel_command{
  [](entity_editor_controller::command_args const &args) {
      args.controller.leave_state();
      return true;
  },
  "Cancel entity add",
};

entity_editor_controller::command_list copy_adjacent_commands{
  {{lmpl::key_code::I}, copy_above_command},
  {{lmpl::key_code::K}, copy_below_command},
  {{lmpl::key_code::J}, copy_left_command},
  {{lmpl::key_code::L}, copy_right_command},
  {{lmpl::key_code::U}, copy_front_command},
  {{lmpl::key_code::O}, copy_behind_command},
  {{lmpl::key_code::Q}, cancel_command},
};

entity_editor_controller::copy_entity_state::copy_entity_state(
  entity_editor_controller::select_state &,
  entity_editor_controller &controller)
    : commands{ranges::views::concat(entity_editor_controller::viewport_commands, copy_adjacent_commands) | ranges::to<command_list>()},
      key_command_map{ranges::views::all(commands) | ranges::to<command_map>()}
{
}
} // namespace lmeditor
