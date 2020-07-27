#include "../visual_registry_controller.h"
#include <range/v3/to_container.hpp>
#include <range/v3/view/concat.hpp>

namespace lmeditor
{
visual_registry_controller::command delete_selected_command{
  [](visual_registry_controller::command_args const &args) {
      if (args.controller.have_selection())
      {
          auto selected_box = args.controller.get_selection();

          lmng::orphan_children(args.map, selected_box);
          args.map.destroy(selected_box);
          args.controller.clear_selection();
          return true;
      }
      return false;
  },
  "Delete selected",
};

visual_registry_controller::command add_command{
  [](visual_registry_controller::command_args const &args) {
      Eigen::Vector3f add_pos = Eigen::Vector3f::Zero();
      if (args.controller.have_selection())
      {
          args.controller
            .template enter_state<
            visual_registry_controller::add_adjacent_state>(
              std::get<visual_registry_controller::select_state>(
                args.controller.state));
          return true;
      }

      entt::entity new_box = args.controller.add_cube(args.map, add_pos, 1.f);

      args.controller.clear_selection();

      return true;
  },
  "Add entity",
};

visual_registry_controller::command deselect_command{
  [](visual_registry_controller::command_args const &args) {
      if (args.controller.have_selection())
      {
          args.controller.clear_selection();
          return true;
      }
      return false;
  },
  "Clear selection",
};

visual_registry_controller::command translate_command{
  [](visual_registry_controller::command_args const &args) {
      if (args.controller.have_selection())
      {
          args.controller
            .template enter_state<visual_registry_controller::translate_state>(
              std::get<visual_registry_controller::select_state>(
                args.controller.state));
          return true;
      }
      else
          return false;
  },
  "Translate entity",
};

visual_registry_controller::command copy_entity_command{
  [](visual_registry_controller::command_args const &args) {
      Eigen::Vector3f add_pos = Eigen::Vector3f::Zero();
      if (args.controller.have_selection())
      {
          args.controller
            .template enter_state<
            visual_registry_controller::copy_entity_state>(
              std::get<visual_registry_controller::select_state>(
                args.controller.state));
          return true;
      }

      return false;
  },
  "Copy entity",
};

visual_registry_controller::command reparent_command{
  [](visual_registry_controller::command_args const &args) {
      if (args.controller.have_selection())
      {
          args.controller
            .template enter_state<visual_registry_controller::reparent_state>(
              std::get<visual_registry_controller::select_state>(
                args.controller.state));
          return true;
      }

      return false;
  },
  "Reparent entity",
};

visual_registry_controller::command scale_command{
  [](visual_registry_controller::command_args const &args) {
      auto selected_box = args.controller.get_selection();
      if (
        args.controller.have_selection() &&
        (args.map.has<lmng::box_render>(selected_box) ||
         args.map.has<lmng::box_collider>(selected_box)))
      {
          args.controller
            .template enter_state<visual_registry_controller::scale_state>(
              std::get<visual_registry_controller::select_state>(
                args.controller.state));
          return true;
      }

      return false;
  },
  "Scale entity",
};

visual_registry_controller::command rotate_command{
  [](visual_registry_controller::command_args const &args) {
      if (args.controller.have_selection())
      {
          args.controller
            .template enter_state<visual_registry_controller::rotate_state>(
              std::get<visual_registry_controller::select_state>(
                args.controller.state));
          return true;
      }

      return false;
  },
  "Rotate entity",
};

visual_registry_controller::command_list select_commands{
  {{lmpl::key_code::X}, delete_selected_command},
  {{lmpl::key_code::A}, add_command},
  {{lmpl::key_code::N}, deselect_command},
  {{lmpl::key_code::M}, translate_command},
  {{lmpl::key_code::C}, copy_entity_command},
  {{lmpl::key_code::Y}, reparent_command},
  {{lmpl::key_code::LeftShift, lmpl::key_code::S}, scale_command},
  {{lmpl::key_code::LeftShift, lmpl::key_code::R}, rotate_command},
};

visual_registry_controller::select_state::select_state(
  visual_registry_controller &controller)
    : commands{ranges::views::concat(visual_registry_controller::move_selection_commands, visual_registry_controller::viewport_commands, select_commands) | ranges::to<command_list>()},
      key_command_map{ranges::views::all(commands) | ranges::to<command_map>()}
{
}
} // namespace lmeditor
