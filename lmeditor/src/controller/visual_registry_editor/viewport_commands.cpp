#include "visual_registry_controller.h"

#include <lmlib/math_constants.h>

namespace lmeditor
{
template <int lat_or_lon, int sign>
visual_registry_controller::command create_orbit_command(char const *name)
{
    return visual_registry_controller::command{
      [](visual_registry_controller::command_args const &args) {
          float amount{0.05f};

          if (args.key_down_event.input_state.key_state.shift())
              amount = lm::pi * 0.25f;

          amount *= sign;

          Eigen::Vector2f lat_lon{0.f, 0.f};
          lat_lon[lat_or_lon] = amount;

          args.controller.camera.rotate(lat_lon);
          return true;
      },
      name,
    };
}
visual_registry_controller::command move_camera_forward_command{
  [](visual_registry_controller::command_args const &args) {
      args.controller.move_camera_closer(
        args.key_down_event.input_state.key_state.shift() ? 0.5f : 0.1f);
      return true;
  },
  "Move camera forward",
};
visual_registry_controller::command move_camera_back_command{
  [](visual_registry_controller::command_args const &args) {
      args.controller.move_camera_further(
        args.key_down_event.input_state.key_state.shift() ? 0.5f : 0.1f);
      return true;
  },
  "Move camera back",
};
visual_registry_controller::command move_to_selection_command{
  [](visual_registry_controller::command_args const &args) {
      if (args.controller.have_selection())
          args.controller.camera.move_to_target(
            lmng::resolve_transform(args.map, args.controller.get_selection())
              .position);
      return true;
  },
  "Move camera to selection",
};

visual_registry_controller::command_list
  visual_registry_controller::viewport_commands{
  {{lmpl::key_code::S}, create_orbit_command<1, 1>("Orbit camera right")},
  {{lmpl::key_code::F}, create_orbit_command<1, -1>("Orbit camera left")},
  {{lmpl::key_code::E}, create_orbit_command<0, 1>("Orbit camera down")},
  {{lmpl::key_code::D}, create_orbit_command<0, -1>("Orbit camera up")},
  {{lmpl::key_code::R}, move_camera_forward_command},
  {{lmpl::key_code::W}, move_camera_back_command},
  {{lmpl::key_code::G}, move_to_selection_command},
};
} // namespace lmeditor
