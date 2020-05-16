#include "entity_list_component.h"
#include <entt/entt.hpp>
#include <lmengine/name.h>
#include <lmtk/vlayout.h>

namespace lmeditor
{
component entity_list_init::operator()()
{
    return std::make_unique<entity_list_component>(*this);
}

entity_list_component::entity_list_component(entity_list_init const &init)
    : controller{init.registry},
      text_material{init.text_material},
      rect_material{init.rect_material},
      font{init.font},
      position{init.position},
      size{init.size},
      selection_background{
        init.renderer,
        init.rect_material,
        init.position,
        {0, 0},
        {0.f, 0.f, 0.f, 1.f},
      }
{
}

void entity_list_component::update_selection_background()
{
    if (controller.named_entities_count == 0)
        return;

    lmtk::text_layout &line = selected_line();

    selection_background.set_rect(
      {position.x, line.position.y}, {size.width, line.get_size().height});
}

lmtk::text_layout &entity_list_component::selected_line()
{
    return line_layouts[controller.selected_entity_index];
}

entity_list_component &entity_list_component::move_resources(
  lmgl::irenderer *renderer,
  lmgl::resource_sink &resource_sink)
{
    selection_background.move_resources(renderer, resource_sink);
    for (auto &layout : line_layouts)
        layout.move_resources(renderer, resource_sink);

    return *this;
}

entity_list_component &entity_list_component::add_to_frame(lmgl::iframe *frame)
{
    update_selection_background();
    selection_background.add_to_frame(frame);
    for (auto &layout : line_layouts)
        layout.render(frame);

    return *this;
}

void entity_list_component::reset(
  lmgl::irenderer &renderer,
  lmgl::resource_sink &resource_sink,
  entt::registry const &registry)
{
    for (auto &layout : line_layouts)
        layout.move_resources(&renderer, resource_sink);

    line_layouts.clear();

    auto layout_factory = lmtk::text_layout_factory{
      renderer, text_material, font, {1.f, 1.f, 1.f}, position};

    registry.view<lmng::name const>().each(
      [&](auto entity, auto &name_component) {
          line_layouts.emplace_back(
            layout_factory.create(name_component.string));
      });

    lmtk::layout_vertical(lmtk::vertical_layout{position.y, 12}, line_layouts);
}

lm::size2i entity_list_component::get_size() { return size; }

lm::point2i entity_list_component::get_position() { return position; }

entity_list_component &
  entity_list_component::set_rect(lm::point2i position, lm::size2i size)
{
    position = position;
    size = size;
    return *this;
}

bool entity_list_component::handle(const lmtk::input_event &input_event)
{
    return controller.handle(input_event);
}

component_interface &entity_list_component::update(
  lmgl::irenderer *renderer,
  lmgl::resource_sink &resource_sink)
{
    if (controller.dirty)
    {
        reset(*renderer, resource_sink, *controller.registry);
        controller.dirty = false;
    }
    return *this;
}

std::vector<command_description>
  entity_list_component::get_command_descriptions()
{
    return std::vector<command_description>();
}
} // namespace lmeditor
