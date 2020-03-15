#include "model.h"

namespace lmeditor
{
map_editor_model::command move_selection_back_command{
  [](map_editor_model::command_args const &args) {
      return args.model.move_selection_view(
        args.map, -Eigen::Vector3f::UnitZ(), args.event_handler);
  },
  "Move selection back",
};

map_editor_model::command move_selection_forward_command{
  [](map_editor_model::command_args const &args) {
      return args.model.move_selection_view(
        args.map, Eigen::Vector3f::UnitZ(), args.event_handler);
  },
  "Move selection forward",
};

map_editor_model::command move_selection_up_command{
  [](map_editor_model::command_args const &args) {
      return args.model.move_selection_view(
        args.map, Eigen::Vector3f::UnitY(), args.event_handler);
  },
  "Move selection up",
};

map_editor_model::command move_selection_down_command{
  [](map_editor_model::command_args const &args) {
      return args.model.move_selection_view(
        args.map, -Eigen::Vector3f::UnitY(), args.event_handler);
  },
  "Move selection down",
};

map_editor_model::command move_selection_left_command{
  [](map_editor_model::command_args const &args) {
      return args.model.move_selection_view(
        args.map, -Eigen::Vector3f::UnitX(), args.event_handler);
  },
  "Move selection left",
};

map_editor_model::command move_selection_right_command{
  [](map_editor_model::command_args const &args) {
      return args.model.move_selection_view(
        args.map, Eigen::Vector3f::UnitX(), args.event_handler);
  },
  "Move selection right",
};

map_editor_model::command_list map_editor_model::move_selection_commands{
  {{lmpl::key_code::U}, move_selection_back_command},
  {{lmpl::key_code::O}, move_selection_forward_command},
  {{lmpl::key_code::I}, move_selection_up_command},
  {{lmpl::key_code::K}, move_selection_down_command},
  {{lmpl::key_code::J}, move_selection_left_command},
  {{lmpl::key_code::L}, move_selection_right_command},
};
} // namespace lmeditor
