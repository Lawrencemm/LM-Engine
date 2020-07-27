#include "../visual_registry_controller.h"
#include <lmlib/eigen.h>
#include <range/v3/to_container.hpp>
#include <range/v3/view/concat.hpp>

namespace lmeditor
{
Eigen::Vector3f get_translation(
  visual_registry_controller &controller,
  entt::registry const &map,
  lmtk::key_down_event const &key_down_event,
  Eigen::Vector3f const &direction)
{
    auto extents = controller.get_selection_extents(map);

    auto snapped_direction = controller.view_to_axis(direction);

    Eigen::Vector3f translation = snapped_direction.cwiseProduct(extents);
    if (!key_down_event.input_state.key_state.shift())
    {
        translation.normalize();
        translation *= 0.01f;
    }
    auto frame = lmng::get_frame(map, controller.get_selection());
    return frame.rotation.inverse() * translation;
}

void do_translation(
  visual_registry_controller::command_args const &args,
  Eigen::Vector3f const &direction)
{
    args.controller.translate(
      args.map,
      args.controller.get_selection(),
      get_translation(
        args.controller, args.map, args.key_down_event, direction));
}

visual_registry_controller::command move_up_command{
  [](visual_registry_controller::command_args const &args) {
      do_translation(args, Eigen::Vector3f::UnitY());
      return true;
  },
  "Move entity up",
};

visual_registry_controller::command move_down_command{
  [](visual_registry_controller::command_args const &args) {
      do_translation(args, -Eigen::Vector3f::UnitY());
      return true;
  },
  "Move entity down",
};

visual_registry_controller::command move_left_command{
  [](visual_registry_controller::command_args const &args) {
      do_translation(args, -Eigen::Vector3f::UnitX());
      return true;
  },
  "Move entity left",
};

visual_registry_controller::command move_right_command{
  [](visual_registry_controller::command_args const &args) {
      do_translation(args, Eigen::Vector3f::UnitX());
      return true;
  },
  "Move entity right",
};

visual_registry_controller::command move_forward_command{
  [](visual_registry_controller::command_args const &args) {
      do_translation(args, -Eigen::Vector3f::UnitZ());
      return true;
  },
  "Move entity forward",
};

visual_registry_controller::command move_back_command{
  [](visual_registry_controller::command_args const &args) {
      do_translation(args, Eigen::Vector3f::UnitZ());
      return true;
  },
  "Move entity back",
};

visual_registry_controller::command quit_move_command{
  [](visual_registry_controller::command_args const &args) {
      args.controller.leave_state();
      return true;
  },
  "Stop moving entity",
};

visual_registry_controller::command_list move_commands{
  {{lmpl::key_code::I}, move_up_command},
  {{lmpl::key_code::K}, move_down_command},
  {{lmpl::key_code::J}, move_left_command},
  {{lmpl::key_code::L}, move_right_command},
  {{lmpl::key_code::U}, move_forward_command},
  {{lmpl::key_code::O}, move_back_command},
  {{lmpl::key_code::Q}, quit_move_command},
};

visual_registry_controller::translate_state::translate_state(
  visual_registry_controller::select_state &,
  visual_registry_controller &)
    : commands{ranges::views::concat(visual_registry_controller::viewport_commands, move_commands) | ranges::to<command_list>()},
      key_command_map{ranges::views::all(commands) | ranges::to<command_map>()}
{
}
} // namespace lmeditor
