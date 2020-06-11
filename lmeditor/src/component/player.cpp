#include <lmeditor/component/player.h>

namespace lmeditor
{
player::player(player_init const &init)
    : registry{init.map.clone()},
      position{init.position},
      size{init.size},
      simulation{init.project_plugin.create_simulation(
        init.simulation_index,
        registry,
        init.asset_cache)},
      visual_view{
        lmhuv::visual_view_init{
          .registry = registry,
          .renderer = init.renderer,
          .aspect_ratio = (float)init.size.width / init.size.height,
        }
          .unique(),
      }
{
}

std::vector<command_description> player::get_command_descriptions()
{
    return std::vector<command_description>();
}

bool player::handle(const lmtk::input_event &input_event)
{
    simulation->handle_input_event(input_event, registry);
    return false;
}

lmtk::component_interface &player::update(
  lmgl::irenderer *renderer,
  lmgl::resource_sink &resource_sink,
  lmtk::resource_cache const &resource_cache,
  lmtk::input_state const &input_state)
{
    simulation->update(registry, clock.tick(), input_state);
    return *this;
}

bool player::add_to_frame(lmgl::iframe *frame)
{
    visual_view->add_to_frame(registry, frame, lmgl::viewport{size});

    return true;
}

lm::size2i player::get_size() { return size; }
lm::point2i player::get_position() { return position; }

lmtk::widget_interface &player::set_rect(lm::point2i position, lm::size2i size)
{
    throw std::runtime_error{"Not implemented: resizing player"};
    return *this;
}

lmtk::widget_interface &
  player::move_resources(lmgl::resource_sink &resource_sink)
{
    visual_view->move_resources(resource_sink);
    return *this;
}
} // namespace lmeditor
