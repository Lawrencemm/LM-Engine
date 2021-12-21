#include "../map_editor_controller.h"
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/concat.hpp>

namespace lmeditor
{
void do_add(
  map_editor_controller::command_args const &args,
  Eigen::Vector3f const &direction)
{
    auto new_box = args.controller.add_adjacent(
      args.map, args.controller.view_to_axis(direction));

    args.controller.select(new_box);

    args.controller.leave_state();
}

map_editor_controller::command add_above_command{
  [](map_editor_controller::command_args const &args) {
      do_add(args, Eigen::Vector3f::UnitY());
      return true;
  },
  "Add entity above",
};

map_editor_controller::command add_below_command{
  [](map_editor_controller::command_args const &args) {
      do_add(args, -Eigen::Vector3f::UnitY());
      return true;
  },
  "Add entity below",
};

map_editor_controller::command add_left_command{
  [](map_editor_controller::command_args const &args) {
      do_add(args, -Eigen::Vector3f::UnitX());
      return true;
  },
  "Add entity left",
};

map_editor_controller::command add_right_command{
  [](map_editor_controller::command_args const &args) {
      do_add(args, Eigen::Vector3f::UnitX());
      return true;
  },
  "Add entity right",
};

map_editor_controller::command add_behind_command{
  [](map_editor_controller::command_args const &args) {
      do_add(args, Eigen::Vector3f::UnitZ());
      return true;
  },
  "Add entity behind",
};

map_editor_controller::command add_front_command{
  [](map_editor_controller::command_args const &args) {
      do_add(args, -Eigen::Vector3f::UnitZ());
      return true;
  },
  "Add entity in front",
};

map_editor_controller::command cancel_command{
  [](map_editor_controller::command_args const &args) {
      args.controller.leave_state();
      return true;
  },
  "Cancel entity add",
};

map_editor_controller::command_list add_adjacent_commands{
  {{lmpl::key_code::I}, add_above_command},
  {{lmpl::key_code::K}, add_below_command},
  {{lmpl::key_code::J}, add_left_command},
  {{lmpl::key_code::L}, add_right_command},
  {{lmpl::key_code::U}, add_front_command},
  {{lmpl::key_code::O}, add_behind_command},
  {{lmpl::key_code::Q}, cancel_command},
};

map_editor_controller::add_adjacent_state::add_adjacent_state(
  map_editor_controller::select_state &,
  map_editor_controller &map_editor)
    : commands{ranges::views::concat(map_editor_controller::viewport_commands, add_adjacent_commands) | ranges::to<command_list>()},
      key_command_map{ranges::views::all(commands) | ranges::to<command_map>()}
{
}
} // namespace lmeditor
