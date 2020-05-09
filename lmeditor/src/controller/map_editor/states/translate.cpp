#include "../map_editor_controller.h"
#include <lmlib/eigen.h>
#include <range/v3/to_container.hpp>
#include <range/v3/view/concat.hpp>

namespace lmeditor
{
Eigen::Vector3f get_translation(
  map_editor_controller &map_editor,
  entt::registry const &map,
  lmtk::key_down_event const &key_down_event,
  Eigen::Vector3f const &direction)
{
    auto extents = map_editor.get_selection_extents(map);

    auto snapped_direction = map_editor.view_to_axis(direction);

    Eigen::Vector3f translation = snapped_direction.cwiseProduct(extents);
    if (!key_down_event.input_state.key_state.shift())
    {
        translation.normalize();
        translation *= 0.01f;
    }
    auto frame = lmng::get_frame(map, map_editor.get_selection());
    return frame.rotation.inverse() * translation;
}

void do_translation(
  map_editor_controller::command_args const &args,
  Eigen::Vector3f const &direction)
{
    args.controller.translate(
      args.map,
      args.controller.get_selection(),
      get_translation(
        args.controller, args.map, args.key_down_event, direction));
}

map_editor_controller::command move_up_command{
  [](map_editor_controller::command_args const &args) {
      do_translation(args, Eigen::Vector3f::UnitY());
      return true;
  },
  "Move entity up",
};

map_editor_controller::command move_down_command{
  [](map_editor_controller::command_args const &args) {
      do_translation(args, -Eigen::Vector3f::UnitY());
      return true;
  },
  "Move entity down",
};

map_editor_controller::command move_left_command{
  [](map_editor_controller::command_args const &args) {
      do_translation(args, -Eigen::Vector3f::UnitX());
      return true;
  },
  "Move entity left",
};

map_editor_controller::command move_right_command{
  [](map_editor_controller::command_args const &args) {
      do_translation(args, Eigen::Vector3f::UnitX());
      return true;
  },
  "Move entity right",
};

map_editor_controller::command move_forward_command{
  [](map_editor_controller::command_args const &args) {
      do_translation(args, -Eigen::Vector3f::UnitZ());
      return true;
  },
  "Move entity forward",
};

map_editor_controller::command move_back_command{
  [](map_editor_controller::command_args const &args) {
      do_translation(args, Eigen::Vector3f::UnitZ());
      return true;
  },
  "Move entity back",
};

map_editor_controller::command quit_move_command{
  [](map_editor_controller::command_args const &args) {
      args.controller.leave_state();
      return true;
  },
  "Stop moving entity",
};

map_editor_controller::command_list move_commands{
  {{lmpl::key_code::I}, move_up_command},
  {{lmpl::key_code::K}, move_down_command},
  {{lmpl::key_code::J}, move_left_command},
  {{lmpl::key_code::L}, move_right_command},
  {{lmpl::key_code::U}, move_forward_command},
  {{lmpl::key_code::O}, move_back_command},
  {{lmpl::key_code::Q}, quit_move_command},
};

map_editor_controller::translate_state::translate_state(
  map_editor_controller::select_state &,
  map_editor_controller &)
    : commands{ranges::views::concat(map_editor_controller::viewport_commands, move_commands) | ranges::to<command_list>()},
      key_command_map{ranges::views::all(commands) | ranges::to<command_map>()}
{
}
} // namespace lmeditor
