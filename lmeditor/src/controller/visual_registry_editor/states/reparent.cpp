#include "../visual_registry_controller.h"
#include <lmng/hierarchy.h>
#include <range/v3/to_container.hpp>
#include <range/v3/view/concat.hpp>

namespace lmeditor
{
visual_registry_controller::command select_parent{
  [](visual_registry_controller::command_args const &args) {
      auto selected_box = args.controller.get_selection();

      auto &reparent_state =
        std::get<visual_registry_controller::reparent_state>(args.controller.state);

      if (selected_box == reparent_state.entity)
          return false;

      lmng::reparent(args.map, reparent_state.entity, selected_box);
      args.controller.leave_state();
      return true;
  },
  "Select new parent",
};

visual_registry_controller::command quit_reparent{
  [](visual_registry_controller::command_args const &args) {
      args.controller.leave_state();
      return true;
  },
  "Cancel reparent",
};

visual_registry_controller::command_list reparent_commands{
  {{lmpl::key_code::Enter}, select_parent},
  {{lmpl::key_code::Q}, quit_reparent},
};

visual_registry_controller::reparent_state::reparent_state(
  visual_registry_controller::select_state &,
  visual_registry_controller &controller)
    : commands{ranges::views::concat(visual_registry_controller::move_selection_commands, visual_registry_controller::viewport_commands, reparent_commands) | ranges::to<command_list>()},
      key_command_map{ranges::views::all(commands) | ranges::to<command_map>()},
      entity{controller.get_selection()}
{
}
} // namespace lmeditor
