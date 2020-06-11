#include "application.h"

#include <yaml-cpp/yaml.h>

#include <lmlib/camera.h>
#include <lmng/serialisation.h>

sample_app::sample_app()
    : resources{},
      flow_graph{
        resources,
        [&](auto &ev) { return on_input_event(ev); },
        [&](auto frame) { return on_new_frame(frame); },
        [&]() { on_quit(); },
      },
      registry{
        lmng::deserialise(YAML::LoadFile("../sample/character_movement.lmap"))},
      simulation{lmng::simulation_init{registry, asset_cache}},
      visual_view{lmhuv::create_visual_view(lmhuv::visual_view_init{
        registry,
        resources.renderer.get(),
      })}
{
    visual_view->set_camera_override(lm::camera{lm::camera_init{
      .fov = (float)M_PI / 3,
      .aspect =
        (float)resources.window_size.width / resources.window_size.height,
      .near_clip = 0.1f,
      .far_clip = 1000.f,
      .position = {0.f, 10.f, -25.f},
      .rotation = Eigen::Quaternionf{Eigen::AngleAxisf{
        (float)M_PI / 8.f, Eigen::Vector3f::UnitX()}},
    }});
}

bool sample_app::on_input_event(lmtk::input_event const &variant)
{
    simulation.handle_input_event(variant, registry);
    return true;
}

bool sample_app::on_new_frame(lmgl::iframe *pIframe)
{
    simulation.update(registry, clock.tick(), resources.input_state);
    visual_view->add_to_frame(
      registry, pIframe, lmgl::viewport{resources.window_size});
    return true;
}

void sample_app::on_quit()
{
    visual_view->move_resources(resources.resource_sink);
}
