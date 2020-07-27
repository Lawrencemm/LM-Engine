#include "visual_registry_controller.h"

namespace lmeditor
{
visual_registry_controller::command move_selection_back_command{
  [](visual_registry_controller::command_args const &args) {
      return args.controller.move_selection_view(
        args.map, -Eigen::Vector3f::UnitZ());
  },
  "Move selection back",
};

visual_registry_controller::command move_selection_forward_command{
  [](visual_registry_controller::command_args const &args) {
      return args.controller.move_selection_view(
        args.map, Eigen::Vector3f::UnitZ());
  },
  "Move selection forward",
};

visual_registry_controller::command move_selection_up_command{
  [](visual_registry_controller::command_args const &args) {
      return args.controller.move_selection_view(
        args.map, Eigen::Vector3f::UnitY());
  },
  "Move selection up",
};

visual_registry_controller::command move_selection_down_command{
  [](visual_registry_controller::command_args const &args) {
      return args.controller.move_selection_view(
        args.map, -Eigen::Vector3f::UnitY());
  },
  "Move selection down",
};

visual_registry_controller::command move_selection_left_command{
  [](visual_registry_controller::command_args const &args) {
      return args.controller.move_selection_view(
        args.map, -Eigen::Vector3f::UnitX());
  },
  "Move selection left",
};

visual_registry_controller::command move_selection_right_command{
  [](visual_registry_controller::command_args const &args) {
      return args.controller.move_selection_view(
        args.map, Eigen::Vector3f::UnitX());
  },
  "Move selection right",
};

visual_registry_controller::command_list
  visual_registry_controller::move_selection_commands{
    {{lmpl::key_code::U}, move_selection_back_command},
    {{lmpl::key_code::O}, move_selection_forward_command},
    {{lmpl::key_code::I}, move_selection_up_command},
    {{lmpl::key_code::K}, move_selection_down_command},
    {{lmpl::key_code::J}, move_selection_left_command},
    {{lmpl::key_code::L}, move_selection_right_command},
  };
} // namespace lmeditor
