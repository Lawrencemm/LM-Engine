#include "../visual_registry_controller.h"
#include <range/v3/to_container.hpp>
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
visual_registry_controller::command create_scale_command(char const *name)
{
    return visual_registry_controller::command{
      [](visual_registry_controller::command_args const &args) {
          Eigen::Vector3f *pextents{nullptr};

          auto selected_box = args.controller.get_selection();

          if (
            auto maybe_box_render =
              args.map.try_get<lmng::box_render>(selected_box))
              pextents = &maybe_box_render->extents;
          else if (
            auto maybe_box_collider =
              args.map.try_get<lmng::box_collider>(selected_box))
              pextents = &maybe_box_collider->extents;

          auto &extents = *pextents;

          float amount{0.01f};

          if (args.key_down_event.input_state.key_state.shift())
              amount = extents[axis] * 0.5f;

          extents[axis] += sign * amount;
          return true;
      },
      name,
    };
}

visual_registry_controller::command quit_scale_command{
  [](visual_registry_controller::command_args const &args) {
      args.controller.leave_state();
      return true;
  },
  "Stop scaling entity",
};

visual_registry_controller::command_list scale_commands{
  {{lmpl::key_code::I}, create_scale_command<1, 1>("Scale up (y)")},
  {{lmpl::key_code::K}, create_scale_command<1, -1>("Scale down (y)")},
  {{lmpl::key_code::J}, create_scale_command<0, -1>("Scale down (x)")},
  {{lmpl::key_code::L}, create_scale_command<0, 1>("Scale up (x)")},
  {{lmpl::key_code::U}, create_scale_command<2, -1>("Scale down (z)")},
  {{lmpl::key_code::O}, create_scale_command<2, 1>("Scale up (z)")},
  {{lmpl::key_code::Q}, quit_scale_command},
};

visual_registry_controller::scale_state::scale_state(
  visual_registry_controller::select_state &,
  visual_registry_controller &)
    : commands{ranges::views::concat(visual_registry_controller::viewport_commands, scale_commands) | ranges::to<command_list>()},
      key_command_map{ranges::views::all(commands) | ranges::to<command_map>()}
{
}
} // namespace lmeditor
