#include "application.h"
#include "lmlib/variant_visitor.h"

#include <yaml-cpp/yaml.h>

#include <lmng/yaml_save_load.h>

sample_app::sample_app()
    : asset_cache{"../assets"},
      registry{lmng::create_registry_from_yaml(
        YAML::LoadFile("../assets/map/character_movement.lmap"),
        asset_cache)},
      simulation{lmng::simulation_init{registry, asset_cache}},
      visual_view{lmhuv::visual_view_init{
        registry,
        resources.renderer.get(),
        resources.window->get_size_client().ratio<float>(),
      }
                    .unique()}
{
}

bool sample_app::on_event(const lmtk::event &event)
{
    event >>
      lm::variant_visitor{
        [&](lmtk::draw_event const &event)
        {
            simulation.update(registry, clock.tick(), event.input_state);
            visual_view->update(registry, &event.renderer, event.resource_sink);
            visual_view->add_to_frame(
              registry,
              &event.frame,
              lmgl::viewport{resources.window->get_size_client()});
        },
        [&](lmtk::quit_event const &event)
        { visual_view->move_resources(resources.resource_sink); },
        [&](auto const &event)
        { simulation.handle_input_event(event, registry); },
      };
    return true;
}
