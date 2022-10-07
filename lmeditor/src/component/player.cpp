#include <lmeditor/component/player.h>
#include <lmng/meta/clone.h>
#include <any>

namespace lmeditor
{
player::player(player_init const &init)
    : registry{lmng::clone(init.map)},
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

bool player::handle(const lmtk::input_event &input_event, std::any model)
{
    simulation->handle_input_event(input_event, registry);
    return false;
}

component_interface &player::update(
  lmgl::irenderer *renderer,
  lmgl::resource_sink &resource_sink,
  lmtk::resource_cache const &resource_cache,
  lmtk::input_state const &input_state,
  std::any model)
{
    simulation->update(registry, clock.tick(), input_state);
    visual_view->update(registry, renderer, resource_sink);
    return *this;
}

bool player::add_to_frame(lmgl::iframe *frame)
{
    visual_view->add_to_frame(registry, frame, lmgl::viewport{position, size});

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
