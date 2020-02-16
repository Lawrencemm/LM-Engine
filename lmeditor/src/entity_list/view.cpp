#include "view.h"

#include <entt/entt.hpp>

#include <lmengine/name.h>
#include <lmtk/vlayout.h>

#include "entity_list.h"

namespace lmeditor
{
entity_list_view::entity_list_view(
  entity_list_init const &init,
  entity_list const &model)
    : text_material{init.text_material},
      rect_material{init.rect_material},
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

void entity_list_view::update_selection_background(entity_list const &model)
{
    if (model.named_entities_count == 0)
        return;

    lmtk::text_layout &line = selected_line(model);

    selection_background.set_rect(
      {position.x, line.position.y}, {size.width, line.get_size().height});
}

lmtk::text_layout &entity_list_view::selected_line(entity_list const &model)
{
    return line_layouts[model.selected_entity_index];
}

void entity_list_view::move_resources(
  lmgl::irenderer *renderer,
  lmtk::resource_sink &resource_sink)
{
    selection_background.move_resources(renderer, resource_sink);
    for (auto &layout : line_layouts)
        layout.move_resources(renderer, resource_sink);
}

void entity_list_view::add_to_frame(
  lmgl::iframe *frame,
  entity_list const &model)
{
    update_selection_background(model);
    selection_background.add_to_frame(frame);
    for (auto &layout : line_layouts)
        layout.render(frame);
}

void entity_list_view::display(
  lmgl::irenderer &renderer,
  lmtk::resource_sink &resource_sink,
  lmtk::ifont const &font,
  entt::registry const &registry,
  entity_list const &model)
{
    for (auto &layout : line_layouts)
        layout.move_resources(&renderer, resource_sink);

    line_layouts.clear();

    auto layout_factory = lmtk::text_layout_factory{
      renderer, text_material, &font, {1.f, 1.f, 1.f}, position};

    registry.view<lmng::name const>().each(
      [&](auto entity, auto &name_component) {
          line_layouts.emplace_back(
            layout_factory.create(name_component.string));
      });

    lmtk::layout_vertical(lmtk::vertical_layout{position.y, 12}, line_layouts);
}
} // namespace lmeditor
