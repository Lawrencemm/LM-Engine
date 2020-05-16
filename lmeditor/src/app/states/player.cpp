#include "../app.h"
#include <lmlib/camera.h>
#include <lmlib/variant_visitor.h>

namespace lmeditor
{
editor_app::player_state editor_app::create_player_state()
{
    auto play_registry = map.clone();
    auto simulation = resources.create_simulation(
      resources.simulation_names[resources.selected_simulation_index],
      lmng::simulation_init{play_registry, project_dir});
    auto player = lmhuv::create_visual_view(lmhuv::visual_view_init{
      .registry = play_registry,
      .renderer = resources.renderer.get(),
      .aspect_ratio =
        (float)resources.window_size.width / resources.window_size.height,
    });
    return player_state{
      .registry = std::move(play_registry),
      .simulation = std::move(simulation),
      .visual_view = std::move(player),
    };
}

bool editor_app::player_state::handle(
  editor_app &app,
  lmtk::input_event const &input_event)
{
    if (
      input_event >> lm::variant_visitor{
                       [&](lmtk::key_down_event const &key_down_event) {
                           switch (key_down_event.key)
                           {
                           case lmpl::key_code::Escape:
                               app.change_state<gui_state>();
                               return true;

                           default:
                               return false;
                           }
                       },
                       [](auto) { return false; },
                     })
        return true;

    simulation->handle_input_event(input_event, registry);
    return true;
}

void editor_app::player_state::update_simulation(
  lmtk::input_state const &input_state)
{
    simulation->update(registry, clock.tick(), input_state);
}

void editor_app::player_state::move_resources(
  lmgl::irenderer *renderer,
  lmgl::resource_sink &resource_sink)
{
    visual_view->move_resources(resource_sink, renderer);
}

void editor_app::player_state::add_to_frame(
  editor_app &app,
  lmgl::iframe *frame)
{
    update_simulation(app.resources.input_state);
    visual_view->add_to_frame(
      registry, frame, lmgl::viewport{app.resources.window_size});
}
} // namespace lmeditor
