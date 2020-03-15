#include "../model.h"

#include <range/v3/view/concat.hpp>

namespace lmeditor
{
map_editor_model::command delete_selected_command{
  [](map_editor_model::command_args const &args) {
      if (args.model.have_selection())
      {
          args.event_handler(
            map_editor_destroying_entity{args.map, args.model.selected_box});
          args.map.destroy(args.model.selected_box);
          args.model.selected_box = entt::null;
          return true;
      }
      return false;
  },
  "Delete selected",
};

map_editor_model::command add_command{
  [](map_editor_model::command_args const &args) {
      Eigen::Vector3f add_pos = Eigen::Vector3f::Zero();
      if (args.model.have_selection())
      {
          args.model.template enter_state<map_editor_model::add_adjacent_state>(
            *static_cast<map_editor_model::select_state *>(args.state_ptr),
            args.on_state_change);
          return true;
      }

      entt::entity new_box = args.model.add_cube(args.map, add_pos, 1.f);
      args.event_handler(map_editor_created_entity{args.map, new_box});

      args.model.selected_box = new_box;
      args.event_handler(map_editor_changed_selection{args.map, new_box});

      return true;
  },
  "Add entity",
};

map_editor_model::command deselect_command{
  [](map_editor_model::command_args const &args) {
      if (args.model.have_selection())
      {
          args.model.selected_box = entt::null;
          args.event_handler(map_editor_cleared_selection{});
          return true;
      }
      return false;
  },
  "Clear selection",
};

map_editor_model::command translate_command{
  [](map_editor_model::command_args const &args) {
      if (args.model.have_selection())
      {
          args.model.template enter_state<map_editor_model::translate_state>(
            *static_cast<map_editor_model::select_state *>(args.state_ptr),
            args.on_state_change);
          return true;
      }
      else
          return false;
  },
  "Translate entity",
};

map_editor_model::command copy_entity_command{
  [](map_editor_model::command_args const &args) {
      Eigen::Vector3f add_pos = Eigen::Vector3f::Zero();
      if (args.model.have_selection())
      {
          args.model.template enter_state<map_editor_model::copy_entity_state>(
            *static_cast<map_editor_model::select_state *>(args.state_ptr),
            args.on_state_change);
          return true;
      }

      return false;
  },
  "Copy entity",
};

map_editor_model::command reparent_command{
  [](map_editor_model::command_args const &args) {
      if (args.model.have_selection())
      {
          args.model.template enter_state<map_editor_model::reparent_state>(
            *static_cast<map_editor_model::select_state *>(args.state_ptr),
            args.on_state_change);
          return true;
      }

      return false;
  },
  "Reparent entity",
};

map_editor_model::command scale_command{
  [](map_editor_model::command_args const &args) {
      if (
        args.model.have_selection() &&
        (args.map.has<lmng::box_render>(args.model.selected_box) ||
         args.map.has<lmng::box_collider>(args.model.selected_box)))
      {
          args.model.template enter_state<map_editor_model::scale_state>(
            *static_cast<map_editor_model::select_state *>(args.state_ptr),
            args.on_state_change);
          return true;
      }

      return false;
  },
  "Scale entity",
};

map_editor_model::command rotate_command{
  [](map_editor_model::command_args const &args) {
      if (args.model.have_selection())
      {
          args.model.template enter_state<map_editor_model::rotate_state>(
            *static_cast<map_editor_model::select_state *>(args.state_ptr),
            args.on_state_change);
          return true;
      }

      return false;
  },
  "Rotate entity",
};

map_editor_model::command_list select_commands{
  {{lmpl::key_code::X}, delete_selected_command},
  {{lmpl::key_code::A}, add_command},
  {{lmpl::key_code::N}, deselect_command},
  {{lmpl::key_code::M}, translate_command},
  {{lmpl::key_code::C}, copy_entity_command},
  {{lmpl::key_code::Y}, reparent_command},
  {{lmpl::key_code::LeftShift, lmpl::key_code::S}, scale_command},
  {{lmpl::key_code::LeftShift, lmpl::key_code::R}, rotate_command},
};

map_editor_model::select_state::select_state(map_editor_model &map_editor)
    : commands{ranges::views::concat(
                 map_editor_model::move_selection_commands,
                 map_editor_model::viewport_commands,
                 select_commands) |
               ranges::to<command_list>()},
      key_command_map{ranges::views::all(commands) | ranges::to<command_map>()}
{
}
} // namespace lmeditor
