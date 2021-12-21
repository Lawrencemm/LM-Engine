#include "entity_editor_controller.h"

namespace lmeditor
{
entity_editor_controller::command move_selection_back_command{
  [](entity_editor_controller::command_args const &args) {
      return args.controller.move_selection_view(
        args.map, -Eigen::Vector3f::UnitZ());
  },
  "Move selection back",
};

entity_editor_controller::command move_selection_forward_command{
  [](entity_editor_controller::command_args const &args) {
      return args.controller.move_selection_view(
        args.map, Eigen::Vector3f::UnitZ());
  },
  "Move selection forward",
};

entity_editor_controller::command move_selection_up_command{
  [](entity_editor_controller::command_args const &args) {
      return args.controller.move_selection_view(
        args.map, Eigen::Vector3f::UnitY());
  },
  "Move selection up",
};

entity_editor_controller::command move_selection_down_command{
  [](entity_editor_controller::command_args const &args) {
      return args.controller.move_selection_view(
        args.map, -Eigen::Vector3f::UnitY());
  },
  "Move selection down",
};

entity_editor_controller::command move_selection_left_command{
  [](entity_editor_controller::command_args const &args) {
      return args.controller.move_selection_view(
        args.map, -Eigen::Vector3f::UnitX());
  },
  "Move selection left",
};

entity_editor_controller::command move_selection_right_command{
  [](entity_editor_controller::command_args const &args) {
      return args.controller.move_selection_view(
        args.map, Eigen::Vector3f::UnitX());
  },
  "Move selection right",
};

entity_editor_controller::command_list
  entity_editor_controller::move_selection_commands{
    {{lmpl::key_code::U}, move_selection_back_command},
    {{lmpl::key_code::O}, move_selection_forward_command},
    {{lmpl::key_code::I}, move_selection_up_command},
    {{lmpl::key_code::K}, move_selection_down_command},
    {{lmpl::key_code::J}, move_selection_left_command},
    {{lmpl::key_code::L}, move_selection_right_command},
  };
} // namespace lmeditor
