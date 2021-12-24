#include "application.h"

#include <yaml-cpp/yaml.h>

#include <lmng/yaml_save_load.h>


sample_app::sample_app()
    : resources{},
      flow_graph{
        resources,
        [&](auto &ev) { return on_input_event(ev); },
        [&](auto frame) { return on_new_frame(frame); },
        [&]() { on_quit(); },
      },
      asset_cache{"../assets"},
      registry{lmng::create_registry_from_yaml(
        YAML::LoadFile("../assets/map/character_movement.lmap"),
        asset_cache)},
      simulation{lmng::simulation_init{registry, asset_cache}},
      visual_view{lmhuv::visual_view_init{
        registry,
        resources.renderer.get(),
        resources.window->get_size_client().ratio<float>(),
      }.unique()}
{
}

bool sample_app::on_input_event(lmtk::input_event const &variant)
{
    simulation.handle_input_event(variant, registry);
    return true;
}

bool sample_app::on_new_frame(lmgl::iframe *pIframe)
{
    simulation.update(registry, clock.tick(), resources.input_state);
    visual_view->update(registry, resources.renderer.get(), resources.resource_sink);
    visual_view->add_to_frame(
      registry, pIframe, lmgl::viewport{resources.window->get_size_client()});
    return true;
}

void sample_app::on_quit()
{
    visual_view->move_resources(resources.resource_sink);
}
