#include "../model.h"
#include <range/v3/view/concat.hpp>

namespace lmeditor
{
map_editor_model::command select_parent{
  [](map_editor_model::command_args const &args) {
      auto &state =
        *reinterpret_cast<map_editor_model::reparent_state *>(args.state_ptr);
      if (args.model.selected_box == state.entity)
          return false;

      lmng::set_transform_parent(
        args.map, state.entity, args.model.selected_box);
      args.model.leave_state(state, args.on_state_change);
      return true;
  },
  "Select new parent",
};

map_editor_model::command quit_reparent{
  [](map_editor_model::command_args const &args) {
      args.model.leave_state(
        *static_cast<map_editor_model::reparent_state *>(args.state_ptr),
        args.on_state_change);
      return true;
  },
  "Cancel reparent",
};

map_editor_model::command_list reparent_commands{
  {{lmpl::key_code::Enter}, select_parent},
  {{lmpl::key_code::Q}, quit_reparent},
};

map_editor_model::reparent_state::reparent_state(
  map_editor_model::select_state &,
  map_editor_model &map_editor)
    : commands{ranges::views::concat(
                 map_editor_model::move_selection_commands,
                 map_editor_model::viewport_commands,
                 reparent_commands) |
               ranges::to<command_list>()},
      key_command_map{ranges::views::all(commands) | ranges::to<command_map>()},
      entity{map_editor.selected_box}
{
}
} // namespace lmeditor
