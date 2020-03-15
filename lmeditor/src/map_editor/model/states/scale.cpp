#include "../model.h"
#include <range/v3/view/concat.hpp>

namespace lmeditor
{
void do_scale(
  entt::registry &registry,
  entt::entity entity,
  int axis,
  float amount)
{
    registry.get<lmng::box_render>(entity).extents[axis] += amount;
}

template <int axis, int sign>
map_editor_model::command create_scale_command(char const *name)
{
    return map_editor_model::command{
      [](map_editor_model::command_args const &args) {
          Eigen::Vector3f *pextents{nullptr};

          if (
            auto maybe_box_render =
              args.model.map.try_get<lmng::box_render>(args.model.selected_box))
              pextents = &maybe_box_render->extents;
          else if (
            auto maybe_box_collider =
              args.model.map.try_get<lmng::box_collider>(
                args.model.selected_box))
              pextents = &maybe_box_collider->extents;

          auto &extents = *pextents;

          float amount{0.01f};

          if (args.key_down_event.input_state.key_state.shift())
              amount = extents[axis] * 0.5f;

          extents[axis] += sign * amount;
          args.event_handler(map_editor_modified_selected{args.model.map});
          return true;
      },
      name,
    };
}

map_editor_model::command quit_scale_command{
  [](map_editor_model::command_args const &args) {
      args.model.leave_state(
        *static_cast<map_editor_model::scale_state *>(args.state_ptr),
        args.on_state_change);
      return true;
  },
  "Stop scaling entity",
};

map_editor_model::command_list scale_commands{
  {{lmpl::key_code::I}, create_scale_command<1, 1>("Scale up (y)")},
  {{lmpl::key_code::K}, create_scale_command<1, -1>("Scale down (y)")},
  {{lmpl::key_code::J}, create_scale_command<0, -1>("Scale down (x)")},
  {{lmpl::key_code::L}, create_scale_command<0, 1>("Scale up (x)")},
  {{lmpl::key_code::U}, create_scale_command<2, -1>("Scale down (z)")},
  {{lmpl::key_code::O}, create_scale_command<2, 1>("Scale up (z)")},
  {{lmpl::key_code::Q}, quit_scale_command},
};

map_editor_model::scale_state::scale_state(
  map_editor_model::select_state &,
  map_editor_model &)
    : commands{ranges::views::concat(
                 map_editor_model::viewport_commands,
                 scale_commands) |
               ranges::to<command_list>()},
      key_command_map{ranges::views::all(commands) | ranges::to<command_map>()}
{
}
} // namespace lmeditor
