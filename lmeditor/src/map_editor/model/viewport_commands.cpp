#include "model.h"

#include <lmlib/math_constants.h>

namespace lmeditor
{
template <int lat_or_lon, int sign>
map_editor_model::command create_orbit_command(char const *name)
{
    return map_editor_model::command{
      [](map_editor_model::command_args const &args) {
          float amount{0.05f};

          if (args.key_down_event.input_state.key_state.shift())
              amount = lm::pi * 0.25f;

          amount *= sign;

          Eigen::Vector2f lat_lon{0.f, 0.f};
          lat_lon[lat_or_lon] = amount;

          args.model.camera.rotate(lat_lon);
          return true;
      },
      name,
    };
}
map_editor_model::command move_camera_forward_command{
  [](map_editor_model::command_args const &args) {
      args.model.move_camera_closer(
        args.key_down_event.input_state.key_state.shift() ? 0.5f : 0.1f);
      return true;
  },
  "Move camera forward",
};
map_editor_model::command move_camera_back_command{
  [](map_editor_model::command_args const &args) {
      args.model.move_camera_further(
        args.key_down_event.input_state.key_state.shift() ? 0.5f : 0.1f);
      return true;
  },
  "Move camera back",
};
map_editor_model::command move_to_selection_command{
  [](map_editor_model::command_args const &args) {
      if (args.model.have_selection())
          args.model.camera.move_to_target(
            lmng::resolve_transform(args.map, args.model.selected_box)
              .position);
      return true;
  },
  "Move camera to selection",
};

map_editor_model::command_list map_editor_model::viewport_commands{
  {{lmpl::key_code::S}, create_orbit_command<1, 1>("Orbit camera right")},
  {{lmpl::key_code::F}, create_orbit_command<1, -1>("Orbit camera left")},
  {{lmpl::key_code::E}, create_orbit_command<0, 1>("Orbit camera down")},
  {{lmpl::key_code::D}, create_orbit_command<0, -1>("Orbit camera up")},
  {{lmpl::key_code::R}, move_camera_forward_command},
  {{lmpl::key_code::W}, move_camera_back_command},
  {{lmpl::key_code::G}, move_to_selection_command},
};
} // namespace lmeditor
