#include "../entity_editor_controller.h"
#include <lmng/hierarchy.h>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/concat.hpp>

namespace lmeditor
{
entity_editor_controller::command select_parent{
  [](entity_editor_controller::command_args const &args) {
      auto selected_box = args.controller.get_selection();

      auto &reparent_state =
        std::get<entity_editor_controller::reparent_state>(args.controller.state);

      if (selected_box == reparent_state.entity)
          return false;

      lmng::reparent(args.map, reparent_state.entity, selected_box);
      args.controller.leave_state();
      return true;
  },
  "Select new parent",
};

entity_editor_controller::command quit_reparent{
  [](entity_editor_controller::command_args const &args) {
      args.controller.leave_state();
      return true;
  },
  "Cancel reparent",
};

entity_editor_controller::command_list reparent_commands{
  {{lmpl::key_code::Enter}, select_parent},
  {{lmpl::key_code::Q}, quit_reparent},
};

entity_editor_controller::reparent_state::reparent_state(
  entity_editor_controller::select_state &,
  entity_editor_controller &controller)
    : commands{ranges::views::concat(entity_editor_controller::move_selection_commands, entity_editor_controller::viewport_commands, reparent_commands) | ranges::to<command_list>()},
      key_command_map{ranges::views::all(commands) | ranges::to<command_map>()},
      entity{controller.get_selection()}
{
}
} // namespace lmeditor
