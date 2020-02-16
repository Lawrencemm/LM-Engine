#pragma once

#include <entt/fwd.hpp>

#include <lmtk/shapes.h>
#include <lmtk/text_layout.h>

namespace lmeditor
{
class entity_list;
struct entity_list_init;

class entity_list_view
{
  public:
    explicit entity_list_view(
      entity_list_init const &init,
      entity_list const &model);

    void display(
      lmgl::irenderer &renderer,
      lmtk::resource_sink &resource_sink,
      lmtk::ifont const &font,
      entt::registry const &registry,
      entity_list const &model);

    lmtk::text_layout &selected_line(entity_list const &model);
    void update_selection_background(entity_list const &model);

    lmgl::material text_material, rect_material;
    lm::point2i position;
    lm::size2i size;
    lmtk::rect selection_background;
    std::vector<lmtk::text_layout> line_layouts;

    void move_resources(
      lmgl::irenderer *renderer,
      lmtk::resource_sink &resource_sink);
    void add_to_frame(lmgl::iframe *frame, entity_list const &model);
};
} // namespace lmeditor
