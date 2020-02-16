#pragma once

#include <entt/fwd.hpp>

#include <lmgl/lmgl.h>
#include <lmtk/font.h>

#include <lmeditor/tool_panel.h>

#include "view.h"

namespace lmeditor
{
struct entity_list_init;

class entity_list : public itool_panel
{
  public:
    explicit entity_list(entity_list_init const &init);

    iwidget &add_to_frame(lmgl::iframe *frame) override;
    lm::size2i get_size() override;
    lm::point2i get_position() override;
    iwidget &set_rect(lm::point2i position, lm::size2i size) override;
    iwidget &move_resources(
      lmgl::irenderer *renderer,
      lmtk::resource_sink &resource_sink) override;
    std::vector<command_description> get_command_descriptions() override;
    entt::entity get_selection(entt::registry const &registry);

    bool handle(lmtk::input_event const &input_event);
    entity_list &display(
      lmgl::irenderer &renderer,
      lmtk::resource_sink &resource_sink,
      lmtk::ifont const &font,
      entt::registry const &registry);

  private:
    friend class entity_list_view;
    int selected_entity_index, named_entities_count;
    entity_list_view view;

    bool handle_key_down(lmtk::key_down_event const &event);
    bool move_selection(int movement);
};

struct entity_list_init
{
    lmgl::irenderer &renderer;
    lmgl::material text_material, rect_material;
    lmtk::ifont const *font;
    lm::point2i position{0, 0};
    lm::size2i size{0, 0};

    entity_list operator()() { return entity_list{*this}; }
};

using pentity_list = lm::reference<entity_list>;
} // namespace lmeditor
