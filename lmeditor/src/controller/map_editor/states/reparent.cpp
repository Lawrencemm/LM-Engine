#include "../map_editor_controller.h"
#include <range/v3/to_container.hpp>
#include <range/v3/view/concat.hpp>

namespace lmeditor
{
map_editor_controller::command select_parent{
  [](map_editor_controller::command_args const &args) {
      auto selected_box = args.controller.get_selection();

      auto &reparent_state =
        std::get<map_editor_controller::reparent_state>(args.controller.state);

      if (selected_box == reparent_state.entity)
          return false;

      lmng::set_transform_parent(args.map, reparent_state.entity, selected_box);
      args.controller.leave_state();
      return true;
  },
  "Select new parent",
};

map_editor_controller::command quit_reparent{
  [](map_editor_controller::command_args const &args) {
      args.controller.leave_state();
      return true;
  },
  "Cancel reparent",
};

map_editor_controller::command_list reparent_commands{
  {{lmpl::key_code::Enter}, select_parent},
  {{lmpl::key_code::Q}, quit_reparent},
};

map_editor_controller::reparent_state::reparent_state(
  map_editor_controller::select_state &,
  map_editor_controller &map_editor)
    : commands{ranges::views::concat(map_editor_controller::move_selection_commands, map_editor_controller::viewport_commands, reparent_commands) | ranges::to<command_list>()},
      key_command_map{ranges::views::all(commands) | ranges::to<command_map>()},
      entity{map_editor.get_selection()}
{
}
} // namespace lmeditor
