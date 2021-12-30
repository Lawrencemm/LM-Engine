#include <lmeditor/component/player.h>
#include <lmng/meta/clone.h>
#include "lmlib/variant_visitor.h"

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

lmtk::component_state player::handle(const lmtk::event &event)
{
    return event >>
           lm::variant_visitor{
             [&](lmtk::draw_event const &draw_event)
             {
                 simulation->update(
                   registry, clock.tick(), draw_event.input_state);
                 visual_view->update(
                   registry, &draw_event.renderer, draw_event.resource_sink);

                 visual_view->add_to_frame(
                   registry, &draw_event.frame, lmgl::viewport{position, size});

                 return lmtk::component_state{0.f};
             },
             [&](auto const &event)
             {
                 simulation->handle_input_event(event, registry);
                 return lmtk::component_state{0.f};
             },
           };
}

lm::size2i player::get_size() { return size; }
lm::point2i player::get_position() { return position; }

lmtk::component_interface &
  player::set_rect(lm::point2i position, lm::size2i size)
{
    throw std::runtime_error{"Not implemented: resizing player"};
    return *this;
}

lmtk::component_interface &
  player::move_resources(lmgl::resource_sink &resource_sink)
{
    visual_view->move_resources(resource_sink);
    return *this;
}
} // namespace lmeditor
