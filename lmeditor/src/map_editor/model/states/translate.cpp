#include "../model.h"

#include <lmlib/eigen.h>

#include <range/v3/view/concat.hpp>

namespace lmeditor
{
Eigen::Vector3f get_translation(
  map_editor_model &map_editor,
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
    auto frame = lmng::get_frame(map, map_editor.selected_box);
    return frame.rotation.inverse() * translation;
}

void do_translation(
  map_editor_model::command_args const &args,
  Eigen::Vector3f const &direction)
{
    args.model.translate(
      args.map,
      args.model.selected_box,
      get_translation(args.model, args.map, args.key_down_event, direction));

    args.event_handler(map_editor_modified_selected{args.map});
}

map_editor_model::command move_up_command{
  [](map_editor_model::command_args const &args) {
      do_translation(args, Eigen::Vector3f::UnitY());
      return true;
  },
  "Move entity up",
};

map_editor_model::command move_down_command{
  [](map_editor_model::command_args const &args) {
      do_translation(args, -Eigen::Vector3f::UnitY());
      return true;
  },
  "Move entity down",
};

map_editor_model::command move_left_command{
  [](map_editor_model::command_args const &args) {
      do_translation(args, -Eigen::Vector3f::UnitX());
      return true;
  },
  "Move entity left",
};

map_editor_model::command move_right_command{
  [](map_editor_model::command_args const &args) {
      do_translation(args, Eigen::Vector3f::UnitX());
      return true;
  },
  "Move entity right",
};

map_editor_model::command move_forward_command{
  [](map_editor_model::command_args const &args) {
      do_translation(args, -Eigen::Vector3f::UnitZ());
      return true;
  },
  "Move entity forward",
};

map_editor_model::command move_back_command{
  [](map_editor_model::command_args const &args) {
      do_translation(args, Eigen::Vector3f::UnitZ());
      return true;
  },
  "Move entity back",
};

map_editor_model::command quit_move_command{
  [](map_editor_model::command_args const &args) {
      args.model.leave_state(
        *static_cast<map_editor_model::translate_state *>(args.state_ptr),
        args.on_state_change);
      return true;
  },
  "Stop moving entity",
};

map_editor_model::command_list move_commands{
  {{lmpl::key_code::I}, move_up_command},
  {{lmpl::key_code::K}, move_down_command},
  {{lmpl::key_code::J}, move_left_command},
  {{lmpl::key_code::L}, move_right_command},
  {{lmpl::key_code::U}, move_forward_command},
  {{lmpl::key_code::O}, move_back_command},
  {{lmpl::key_code::Q}, quit_move_command},
};

map_editor_model::translate_state::translate_state(
  map_editor_model::select_state &,
  map_editor_model &)
    : commands{ranges::views::concat(
                 map_editor_model::viewport_commands,
                 move_commands) |
               ranges::to<command_list>()},
      key_command_map{ranges::views::all(commands) | ranges::to<command_map>()}
{
}
} // namespace lmeditor
